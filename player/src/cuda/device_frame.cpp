#include "precompiled.h"
#include "device_frame.h"
#include "decoder.h"

namespace yaga {
namespace player {
namespace {

// ------------------------------------------------------------------------------------------------------------------------------
void getFrameMaxSize(CUVIDEOFORMAT* format, uint32_t& maxWidth, uint32_t& maxHeight)
{
  // AV1 has max width/height of sequence in sequence header
  if (format->codec == cudaVideoCodec_AV1 && format->seqhdr_data_length > 0) {
    if (format->coded_width >= maxWidth || format->coded_height >= maxHeight) {
      auto formatEx = (CUVIDEOFORMATEX*)format;
      maxWidth  = formatEx->av1.max_width;
      maxHeight = formatEx->av1.max_height;
    }
  }
  if (maxWidth < format->coded_width) {
    maxWidth = format->coded_width;
  }
  if (maxHeight < format->coded_height) {
    maxHeight = format->coded_height;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
const char* toString(cudaVideoCodec codec)
{
  struct CodecName
  {
    cudaVideoCodec index;
    const char* name;
  };
  static CodecName codecs [] = {
    { cudaVideoCodec_MPEG1,     "MPEG-1"       },
    { cudaVideoCodec_MPEG2,     "MPEG-2"       },
    { cudaVideoCodec_MPEG4,     "MPEG-4 (ASP)" },
    { cudaVideoCodec_VC1,       "VC-1/WMV"     },
    { cudaVideoCodec_H264,      "AVC/H.264"    },
    { cudaVideoCodec_JPEG,      "M-JPEG"       },
    { cudaVideoCodec_H264_SVC,  "H.264/SVC"    },
    { cudaVideoCodec_H264_MVC,  "H.264/MVC"    },
    { cudaVideoCodec_HEVC,      "H.265/HEVC"   },
    { cudaVideoCodec_VP8,       "VP8"          },
    { cudaVideoCodec_VP9,       "VP9"          },
    { cudaVideoCodec_AV1,       "AV1"          },
    { cudaVideoCodec_NumCodecs, "Invalid"      },
    { cudaVideoCodec_YUV420,    "YUV  4:2:0"   },
    { cudaVideoCodec_YV12,      "YV12 4:2:0"   },
    { cudaVideoCodec_NV12,      "NV12 4:2:0"   },
    { cudaVideoCodec_YUYV,      "YUYV 4:2:2"   },
    { cudaVideoCodec_UYVY,      "UYVY 4:2:2"   },
  };
  if (codec >= 0 && codec <= cudaVideoCodec_NumCodecs) {
    return codecs[codec].name;
  }
  for (int i = cudaVideoCodec_NumCodecs + 1; i < sizeof(codecs) / sizeof(codecs[0]); ++i) {
    if (codec == codecs[i].index) return codecs[codec].name;
  }
  return "Unknown";
}

// ------------------------------------------------------------------------------------------------------------------------------
bool formatSupported(const CUVIDDECODECAPS& capacities, const cudaVideoSurfaceFormat& format)
{
  return capacities.nOutputFormatMask & (1 << format);
}

// ------------------------------------------------------------------------------------------------------------------------------
cudaVideoSurfaceFormat pickOutputFormat(CUVIDEOFORMAT* format, const CUVIDDECODECAPS& capacities)
{
  cudaVideoSurfaceFormat outputFormat{};
  switch (format->chroma_format) {
    case cudaVideoChromaFormat_Monochrome:
    case cudaVideoChromaFormat_420:
      outputFormat = format->bit_depth_luma_minus8
        ? cudaVideoSurfaceFormat_P016
        : cudaVideoSurfaceFormat_NV12;
      break;
    case cudaVideoChromaFormat_444:
      outputFormat = format->bit_depth_luma_minus8
        ? cudaVideoSurfaceFormat_YUV444_16Bit
        : cudaVideoSurfaceFormat_YUV444;
      break;
    case cudaVideoChromaFormat_422:
      // no 4:2:2 output format supported yet so make 420 default
    default:
      outputFormat = cudaVideoSurfaceFormat_NV12;
      break;
  }
  if (!formatSupported(capacities, outputFormat)) {
    if (formatSupported(capacities, cudaVideoSurfaceFormat_NV12)) {
      outputFormat = cudaVideoSurfaceFormat_NV12;
    }
    else if (formatSupported(capacities, cudaVideoSurfaceFormat_P016)) {
      outputFormat = cudaVideoSurfaceFormat_P016;
    }
    else if (formatSupported(capacities, cudaVideoSurfaceFormat_YUV444)) {
      outputFormat = cudaVideoSurfaceFormat_YUV444;
    }
    else if (formatSupported(capacities, cudaVideoSurfaceFormat_YUV444_16Bit)) {
      outputFormat = cudaVideoSurfaceFormat_YUV444_16Bit;
    }
    else {
      THROW("Could not find supported output format");
    }
  }
  return outputFormat;
}

// ------------------------------------------------------------------------------------------------------------------------------
CUVIDDECODECAPS getCapacities(CudaContext* cuda, CUVIDEOFORMAT* format)
{
  CUVIDDECODECAPS caps{};
  caps.eCodecType      = format->codec;
  caps.eChromaFormat   = format->chroma_format;
  caps.nBitDepthMinus8 = format->bit_depth_luma_minus8;
  {
    ContextLock lock(cuda);
    NV_DECODER_CALL(cuvidGetDecoderCaps(&caps));
  }
  if (!caps.bIsSupported) {
    THROW("Codec \"%1%\" is not supported", toString(format->codec));
  }
  if (format->coded_width > caps.nMaxWidth || format->coded_height > caps.nMaxHeight) {
    THROW("Resolution %1%x%2% is not supported (max %3%x%4%)", 
      format->coded_width,
      format->coded_height,
      caps.nMaxWidth,
      caps.nMaxHeight
    );
  }
  if (const unsigned maxMb = (format->coded_width >> 4) * (format->coded_height >> 4) > caps.nMaxMBCount) {
    THROW("Bandwidth %1% is not supported, max %2%", maxMb, caps.nMaxMBCount);
  }
  return caps;
}

// ------------------------------------------------------------------------------------------------------------------------------
void checkDecodeStatus(CUvideodecoder decoder, CUVIDPARSERDISPINFO* picture)
{
  CUVIDGETDECODESTATUS status {};
  CUresult result = cuvidGetDecodeStatus(decoder, picture->picture_index, &status);
  if (result != CUDA_SUCCESS) return;
  if (status.decodeStatus == cuvidDecodeStatus_Error ||
    status.decodeStatus == cuvidDecodeStatus_Error_Concealed)
  {
    LOG(error) << "Cuda decoding error";
  }
}

// ------------------------------------------------------------------------------------------------------------------------------
CUdeviceptr mapVideoFrame(CUvideodecoder decoder, CUVIDPARSERDISPINFO* picture, CUstream stream, unsigned* pitch)
{
  CUdeviceptr ptr {};
  CUVIDPROCPARAMS mapParams {};
  mapParams.progressive_frame = picture->progressive_frame;
  mapParams.second_field      = picture->repeat_first_field + 1;
  mapParams.top_field_first   = picture->top_field_first;
  mapParams.unpaired_field    = picture->repeat_first_field < 0;
  mapParams.output_stream     = stream;
  NV_DECODER_CALL(cuvidMapVideoFrame(decoder, picture->picture_index, &ptr, pitch, &mapParams));
  checkDecodeStatus(decoder, picture);
  return ptr;
}

// ------------------------------------------------------------------------------------------------------------------------------
float getChromaHeightFactor(cudaVideoSurfaceFormat format)
{
  switch (format) {
  case cudaVideoSurfaceFormat_NV12:
  case cudaVideoSurfaceFormat_P016:
    return 0.5f;
  case cudaVideoSurfaceFormat_YUV444:
  case cudaVideoSurfaceFormat_YUV444_16Bit:
    return 1.0f;
  }
  return 0.5;
}

// ------------------------------------------------------------------------------------------------------------------------------
int getChromaPlaneCount(cudaVideoSurfaceFormat format)
{
  switch (format) {
  case cudaVideoSurfaceFormat_NV12:
  case cudaVideoSurfaceFormat_P016:
    return 1;
  case cudaVideoSurfaceFormat_YUV444:
  case cudaVideoSurfaceFormat_YUV444_16Bit:
    return 2;
  }
  return 1;
}

// -----------------------------------------------------------------------------------------------------------------------------
uint32_t round2(uint32_t value)
{
  return (value + 1) & 0xfffffffe;
}

// ------------------------------------------------------------------------------------------------------------------------------
uint32_t getFrameWidth(const cudaVideoSurfaceFormat& format, uint32_t width)
{
  return format == (cudaVideoSurfaceFormat_NV12 || format == cudaVideoSurfaceFormat_P016)
    ? round2(width)
    : width;
}

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<CUvideoctxlock> createLock(CudaContext* cuda)
{
  CUvideoctxlock lock{};
  NV_DECODER_CALL(cuvidCtxLockCreate(&lock, **cuda));
  auto destroyContext = [](CUvideoctxlock lock) {
    cuvidCtxLockDestroy(lock);
  };
  return AutoDestructor<CUvideoctxlock>(lock, destroyContext);
}

typedef enum ColorSpaceStandard {
  ColorSpaceStandard_BT709 = 1,
  ColorSpaceStandard_Unspecified = 2,
  ColorSpaceStandard_Reserved = 3,
  ColorSpaceStandard_FCC = 4,
  ColorSpaceStandard_BT470 = 5,
  ColorSpaceStandard_BT601 = 6,
  ColorSpaceStandard_SMPTE240M = 7,
  ColorSpaceStandard_YCgCo = 8,
  ColorSpaceStandard_BT2020 = 9,
  ColorSpaceStandard_BT2020C = 10
} ColorSpaceStandard;


// -----------------------------------------------------------------------------------------------------------------------------
void getColorSpaceConstants(int colorSpace, float &wr, float &wb, int &black, int &white, int &max)
{
  black = 16; white = 235;
  max = 255;
  switch (colorSpace) {
    case ColorSpaceStandard_BT709:
    default:
      wr = 0.2126f; wb = 0.0722f;
      break;

    case ColorSpaceStandard_FCC:
      wr = 0.30f; wb = 0.11f;
      break;

    case ColorSpaceStandard_BT470:
    case ColorSpaceStandard_BT601:
      wr = 0.2990f; wb = 0.1140f;
      break;

    case ColorSpaceStandard_SMPTE240M:
      wr = 0.212f; wb = 0.087f;
      break;

    case ColorSpaceStandard_BT2020:
    case ColorSpaceStandard_BT2020C:
      wr = 0.2627f; wb = 0.0593f;
      // 10-bit only
      black = 64 << 6; white = 940 << 6;
      max = (1 << 16) - 1;
      break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void getYuv2RgbMatrix(int colorSpace, float* mat)
{
  float wr, wb;
  int black, white, max;
  getColorSpaceConstants(colorSpace, wr, wb, black, white, max);
  float matrix[3][3] = {
    1.0f, 0.0f, (1.0f - wr) / 0.5f,
    1.0f, -wb * (1.0f - wb) / 0.5f / (1 - wb - wr), -wr * (1 - wr) / 0.5f / (1 - wb - wr),
    1.0f, (1.0f - wb) / 0.5f, 0.0f,
  };
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      matrix[i][j] = (float)(1.0 * max / (white - black) * matrix[i][j]);
    }
  }
  memcpy(mat, matrix, sizeof(float) * 9);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
DeviceFrame::DeviceFrame(CudaContext* cuda, uint32_t width, uint32_t height) :
  configured_(false),
  cuda_(cuda),
  format_{},
  contextLock_(createLock(cuda)),
  outputFormat_{},
  dimWidth_(width),
  dimHeight_(height),
  frameWidth_(0),
  lumaHeight_(0),
  chromaHeight_(0),
  maxWidth_(0),
  maxHeight_(0),
  frameSize_(0),
  bytesPerPixel_(0),
  chromaPlaneCount_(0)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
DeviceFrame::~DeviceFrame()
{
}

// ------------------------------------------------------------------------------------------------------------------------------
int DeviceFrame::changeFormat(CUVIDEOFORMAT* format)
{
  if (!format) {
    decoder_.set();
    return createDecoder(&format_);
  }
  if (configured_) {
    return reconfigureDecoder(format);
  }
  return createDecoder(format);
}

// ------------------------------------------------------------------------------------------------------------------------------
int DeviceFrame::decode(CUVIDPICPARAMS* picture)
{
  ContextLock lock(cuda_);
  NV_DECODER_CALL(cuvidDecodePicture(*decoder_, picture));
  return 1;
}

// ------------------------------------------------------------------------------------------------------------------------------
void DeviceFrame::map(CUVIDPARSERDISPINFO* picture, VideoPlayerFrame* frame)
{
  ContextLock lock(cuda_);
  CUstream stream {};
  unsigned pitch = 0;
  auto deviceFrame = mapVideoFrame(*decoder_, picture, stream, &pitch);
  
  CUDA_MEMCPY2D copyParams {};
  copyParams.srcMemoryType = CU_MEMORYTYPE_DEVICE;
  copyParams.dstMemoryType = CU_MEMORYTYPE_HOST;
  copyParams.WidthInBytes  = static_cast<size_t>(frameWidth_) * bytesPerPixel_;
  copyParams.srcPitch      = pitch;
  copyParams.dstPitch      = copyParams.WidthInBytes;
  if (frame->size() != frameSize_) {
    frame->size(frameSize_);
  }
  copyLumaData  (copyParams, stream, deviceFrame, frame->data());
  copyChromaData(copyParams, stream, deviceFrame, frame->data());
  frame->timestamp(static_cast<int64_t>(picture->timestamp));
  
  CUDA_DRIVER_CALL(cuStreamSynchronize(stream));
  NV_DECODER_CALL(cuvidUnmapVideoFrame(*decoder_, deviceFrame));

  
  float matrix[9] {};
  getYuv2RgbMatrix(format_.video_signal_description.matrix_coefficients, matrix);
}

// ------------------------------------------------------------------------------------------------------------------------------
int DeviceFrame::createDecoder(CUVIDEOFORMAT* format)
{
  const auto capacities = getCapacities(cuda_, format);
  bytesPerPixel_ = format->bit_depth_luma_minus8 > 0 ? 2 : 1;
  format_        = *format;
  outputFormat_  = pickOutputFormat(format, capacities);
  chromaPlaneCount_ = getChromaPlaneCount(outputFormat_);
  getFrameMaxSize(format, maxWidth_, maxHeight_);
  calcFrameSize();
  CUVIDDECODECREATEINFO info {};
  info.CodecType           = format->codec;
  info.ChromaFormat        = format->chroma_format;
  info.OutputFormat        = outputFormat_;
  info.bitDepthMinus8      = format->bit_depth_luma_minus8;
  info.DeinterlaceMode     = format->progressive_sequence 
                               ? cudaVideoDeinterlaceMode_Weave
                               : cudaVideoDeinterlaceMode_Adaptive;
  info.ulNumOutputSurfaces = 2;
  info.ulCreationFlags     = cudaVideoCreate_PreferCUVID;
  info.ulNumDecodeSurfaces = format->min_num_decode_surfaces;
  info.vidLock             = *contextLock_;
  info.ulWidth             = format->coded_width;
  info.ulHeight            = format->coded_height;
  info.ulMaxWidth          = maxWidth_;
  info.ulMaxHeight         = maxHeight_;
  info.display_area.left   = format->display_area.left;
  info.display_area.top    = format->display_area.top;
  info.display_area.right  = format->display_area.right;
  info.display_area.bottom = format->display_area.bottom;
  info.ulTargetWidth       = dimWidth_;
  info.ulTargetHeight      = dimHeight_;

  ContextLock lock(cuda_);
  CUvideodecoder decoder{};
  NV_DECODER_CALL(cuvidCreateDecoder(&decoder, &info));
  auto destroyDecoder = [cuda = cuda_](CUvideodecoder decoder) {
    ContextLock lock(cuda);
    cuvidDestroyDecoder(decoder);
  };
  decoder_ = AutoDestructor<CUvideodecoder>(decoder, destroyDecoder);
  return format->min_num_decode_surfaces;
}

// ------------------------------------------------------------------------------------------------------------------------------
void DeviceFrame::resize(uint32_t width, uint32_t height)
{
  dimWidth_  = width;
  dimHeight_ = height;
}

// ------------------------------------------------------------------------------------------------------------------------------
int DeviceFrame::reconfigureDecoder(CUVIDEOFORMAT* format)
{
  if (format->bit_depth_luma_minus8 != format_.bit_depth_luma_minus8 || 
    format->bit_depth_chroma_minus8 != format_.bit_depth_chroma_minus8)
  {
    THROW("Video decoder cannot change bit depth");
  }
  if (format->chroma_format != format_.chroma_format) {
    THROW("Video decoder cannot change chroma format");
  }
  if ((format->coded_width > maxWidth_ || format->coded_height > maxHeight_) && format->codec != cudaVideoCodec_VP9) {
    THROW("Reconfigure Not supported when width/height > maxwidth/maxheight", CUDA_ERROR_NOT_SUPPORTED);
  }
  /*const bool rectChange =
    format->coded_width         != format_.coded_width         || 
    format->coded_height        != format_.coded_height        ||
    format->display_area.bottom != format_.display_area.bottom ||
    format->display_area.top    != format_.display_area.top    ||
    format->display_area.left   != format_.display_area.left   ||
    format->display_area.right  != format_.display_area.right;
  if (!rectChange) return 1;*/
  format_ = *format;
  calcFrameSize();
  CUVIDRECONFIGUREDECODERINFO info {};
  info.ulWidth             = format->coded_width;
  info.ulHeight            = format->coded_height;
  info.display_area.bottom = format_.display_area.bottom;
  info.display_area.top    = format_.display_area.top;
  info.display_area.left   = format_.display_area.left;
  info.display_area.right  = format_.display_area.right;
  info.ulNumDecodeSurfaces = format->min_num_decode_surfaces;
  info.ulTargetWidth       = dimWidth_;
  info.ulTargetHeight      = dimHeight_;
  ContextLock lock(cuda_);
  NV_DECODER_CALL(cuvidReconfigureDecoder(&decoder_, &info));
  return format->min_num_decode_surfaces;
}

// ------------------------------------------------------------------------------------------------------------------------------
void DeviceFrame::calcFrameSize()
{
  frameWidth_        = getFrameWidth(outputFormat_, dimWidth_);
  lumaHeight_        = round2(dimHeight_);
  chromaHeight_      = static_cast<uint32_t>(ceil(lumaHeight_ * getChromaHeightFactor(outputFormat_)));
  const auto height_ = static_cast<size_t>(lumaHeight_) + static_cast<size_t>(chromaHeight_) * chromaPlaneCount_;
  frameSize_         = height_ * frameWidth_ * bytesPerPixel_;
}

// ------------------------------------------------------------------------------------------------------------------------------
void DeviceFrame::copyLumaData(CUDA_MEMCPY2D& params, CUstream stream, CUdeviceptr from, char* to)
{
  params.srcDevice = from;
  params.dstHost   = to;
  params.dstDevice = (CUdeviceptr)to;
  params.Height    = lumaHeight_;
  CUDA_DRIVER_CALL(cuMemcpy2DAsync(&params, stream));
}

// ------------------------------------------------------------------------------------------------------------------------------
void DeviceFrame::copyChromaData(CUDA_MEMCPY2D& params, CUstream stream, CUdeviceptr from, char* to)
{
  const size_t srcOffset = params.srcPitch * lumaHeight_;
  const size_t dstOffset = params.dstPitch * lumaHeight_;
  params.srcDevice = (CUdeviceptr)((char*)from + srcOffset);
  params.dstHost   = to + dstOffset;
  params.dstDevice = (CUdeviceptr)params.dstHost;
  params.Height    = chromaHeight_;
  CUDA_DRIVER_CALL(cuMemcpy2DAsync(&params, stream));
  // TODO ????
  if (chromaPlaneCount_ == 2) {
    params.srcDevice = (CUdeviceptr)((uint8_t*)from + srcOffset * 2);
    params.dstHost   = to + dstOffset * 2;
    params.dstDevice = (CUdeviceptr)params.dstHost;
    CUDA_DRIVER_CALL(cuMemcpy2DAsync(&params, stream));
  }
}

} // !namespace player
} // !namespace yaga
