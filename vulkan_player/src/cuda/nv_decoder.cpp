#include "precompiled.h"
#include "nv_decoder.h"

#define DECODER(data) static_cast<NvDecoder*>(userData)
#define SZ(v) static_cast<size_t>(v)

namespace yaga {
namespace nv_decoder_detail {

// -----------------------------------------------------------------------------------------------------------------------------
int CUDAAPI changeFormat(void* userData, CUVIDEOFORMAT* format)
{
  return DECODER(userData)->changeFormat(format);
}

// -----------------------------------------------------------------------------------------------------------------------------
int CUDAAPI decodePicture(void* userData, CUVIDPICPARAMS* pictureParams)
{ 
  return DECODER(userData)->decodePicture(pictureParams);
}

// -----------------------------------------------------------------------------------------------------------------------------
int CUDAAPI displayPicture(void* userData, CUVIDPARSERDISPINFO* displayInfo)
{
  return DECODER(userData)->displayPicture(displayInfo);
}

// -----------------------------------------------------------------------------------------------------------------------------
int CUDAAPI getOperatingPoint(void* userData, CUVIDOPERATINGPOINTINFO* operatingPointInfo)
{
  return DECODER(userData)->getOperatingPoint(operatingPointInfo);
}

} // !namespace nv_decoder_detail

namespace {

// -----------------------------------------------------------------------------------------------------------------------------
cudaVideoCodec getNvCodecId(AVCodecID id)
{
  switch (id) {
    case AV_CODEC_ID_MPEG1VIDEO: return cudaVideoCodec_MPEG1;
    case AV_CODEC_ID_MPEG2VIDEO: return cudaVideoCodec_MPEG2;
    case AV_CODEC_ID_MPEG4:      return cudaVideoCodec_MPEG4;
    case AV_CODEC_ID_WMV3:
    case AV_CODEC_ID_VC1:        return cudaVideoCodec_VC1;
    case AV_CODEC_ID_H264:       return cudaVideoCodec_H264;
    case AV_CODEC_ID_HEVC:       return cudaVideoCodec_HEVC;
    case AV_CODEC_ID_VP8:        return cudaVideoCodec_VP8;
    case AV_CODEC_ID_VP9:        return cudaVideoCodec_VP9;
    case AV_CODEC_ID_MJPEG:      return cudaVideoCodec_JPEG;
    case AV_CODEC_ID_AV1:        return cudaVideoCodec_AV1;
    default: THROW("Unknown video codec: %1%", (int)id);
  }
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
CUVIDDECODECAPS getCapacities(CudaContext& cuda, CUVIDEOFORMAT* format)
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
CUdeviceptr mapVideoFrame(CUvideodecoder decoder, CUVIDPARSERDISPINFO* picture, CUstream stream, size_t* pitch)
{
  unsigned pitch_ = 0;
  CUdeviceptr ptr {};
  CUVIDPROCPARAMS mapParams {};
  mapParams.progressive_frame = picture->progressive_frame;
  mapParams.second_field      = picture->repeat_first_field + 1;
  mapParams.top_field_first   = picture->top_field_first;
  mapParams.unpaired_field    = picture->repeat_first_field < 0;
  mapParams.output_stream     = stream;
  NV_DECODER_CALL(cuvidMapVideoFrame(decoder, picture->picture_index, &ptr, &pitch_, &mapParams));
  checkDecodeStatus(decoder, picture);
  *pitch = pitch_;
  return ptr;
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

namespace cspace {

enum ColorSpace {
  BT709       = 1,
  Unspecified = 2,
  Reserved    = 3,
  FCC         = 4,
  BT470       = 5,
  BT601       = 6,
  SMPTE240M   = 7,
  YCgCo       = 8,
  BT2020      = 9,
  BT2020C     = 10
};

} // !namespace color_space

// -----------------------------------------------------------------------------------------------------------------------------
void getColorSpaceConstants(int colorSpace, float& wr, float& wb, int& black, int& white, int& max)
{
  black = 16;
  white = 235;
  max   = 255;
  switch (colorSpace) {
  case cspace::BT709:
    default:
      wr = 0.2126f;
      wb = 0.0722f;
      break;
    case cspace::FCC:
      wr = 0.30f;
      wb = 0.11f;
      break;
    case cspace::BT470:
    case cspace::BT601:
      wr = 0.2990f;
      wb = 0.1140f;
      break;
    case cspace::SMPTE240M:
      wr = 0.212f;
      wb = 0.087f;
      break;
    case cspace::BT2020:
    case cspace::BT2020C:
      wr = 0.2627f;
      wb = 0.0593f;
      // 10-bit only
      black = 64 << 6;
      white = 940 << 6;
      max = (1 << 16) - 1;
      break;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void getYuv2RgbMatrix(int colorSpace, float* matrix)
{
  float wr  = 0.0f;
  float wb  = 0.0f;
  int black = 16;
  int white = 235;
  int max   = 255;
  getColorSpaceConstants(colorSpace, wr, wb, black, white, max);
  const float wmb = static_cast<float>(white) - black;
  matrix[0] = 1.0f;
  matrix[1] = 0.0f;
  matrix[2] = (1.0f - wr) / 0.5f;
  matrix[3] = 1.0f;
  matrix[4] = -wb * (1.0f - wb) / 0.5f / (1 - wb - wr);
  matrix[5] = -wr * (1 - wr) / 0.5f / (1 - wb - wr);
  matrix[6] = 1.0f;
  matrix[7] = (1.0f - wb) / 0.5f;
  matrix[8] = 0.0f;
  for (int i = 0; i < 9; ++i) {
    matrix[i] *= max / wmb;
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
DecoderBackendPtr createDecoderBackend(
  AVCodecID      codec, 
  unsigned       clockRate, 
  uint32_t       width, 
  uint32_t       height, 
  DecoderBackend::Handler handler)
{
  return std::make_unique<NvDecoder>(getNvCodecId(codec), clockRate, width, height, handler);
}

// -----------------------------------------------------------------------------------------------------------------------------
NvDecoder::NvDecoder(
  cudaVideoCodec codec,
  unsigned       clockRate,
  uint32_t       width,
  uint32_t       height, 
  DecoderBackend::Handler handler
) :
  cuda_{},
  operatingPoint_(),
  displayAllLayers_(false),
  handler_(handler),
  parser_{},
  configured_(false),
  format_{},
  contextLock_(createLock(&cuda_)),
  outputFormat_{},
  dimWidth_(width),
  dimHeight_(height),
  maxWidth_(0),
  maxHeight_(0)
{
  createParser(codec, clockRate);
}

// ------------------------------------------------------------------------------------------------------------------------------
NvDecoder::~NvDecoder()
{
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::createParser(cudaVideoCodec codec, unsigned clockRate)
{
  CUVIDPARSERPARAMS params{};
  params.CodecType              = codec;
  params.ulMaxNumDecodeSurfaces = 1;
  params.ulClockRate            = clockRate;
  params.ulMaxDisplayDelay      = 1;
  params.pUserData              = this;
  params.pfnSequenceCallback    = nv_decoder_detail::changeFormat;
  params.pfnDecodePicture       = nv_decoder_detail::decodePicture;
  params.pfnDisplayPicture      = nv_decoder_detail::displayPicture;
  params.pfnGetOperatingPoint   = nv_decoder_detail::getOperatingPoint;
  CUvideoparser parser;
  NV_DECODER_CALL(cuvidCreateVideoParser(&parser, &params));
  auto deleteParser = [](CUvideoparser p) {
    LOG(trace) << "Cuvid Video Parser destroied";
    cuvidDestroyVideoParser(p);
  };
  parser_.set(parser, deleteParser);
  LOG(trace) << "Cuvid Video Parser created";
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::decode(const uint8_t* data, uint64_t size, int flags, int64_t timestamp)
{
  CUVIDSOURCEDATAPACKET params{};
  params.payload      = data;
  params.payload_size = static_cast<unsigned long>(size);
  params.flags        = flags | CUVID_PKT_TIMESTAMP; 
  params.timestamp    = timestamp;
  if (!data || size == 0) {
    params.flags |= CUVID_PKT_ENDOFSTREAM;
  }
  NV_DECODER_CALL(cuvidParseVideoData(*parser_, &params));
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::resize(uint32_t width, uint32_t height)
{
  dimWidth_  = width;
  dimHeight_ = height;
  reconfigureDecoder(&format_);
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::getOperatingPoint(CUVIDOPERATINGPOINTINFO* opInfo)
{
  if (opInfo->codec == cudaVideoCodec_AV1 && opInfo->av1.operating_points_cnt > 1) {
    if (operatingPoint_ >= opInfo->av1.operating_points_cnt) {
      operatingPoint_ = 0;
    }
    return operatingPoint_ | (displayAllLayers_ << 10);
  }
  return -1;
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::changeFormat(CUVIDEOFORMAT* format)
{
  return configured_ ? reconfigureDecoder(format) : createDecoder(format);
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::decodePicture(CUVIDPICPARAMS* picture)
{
  ContextLock lock(cuda_);
  NV_DECODER_CALL(cuvidDecodePicture(*decoder_, picture));
  return 1;
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::displayPicture(CUVIDPARSERDISPINFO* picture)
{
  ContextLock lock(cuda_);
  handler_([this, picture](Frame* frame) {
    CUstream stream {};
    size_t pitch = 0;
    auto deviceFrame = mapVideoFrame(*decoder_, picture, stream, &pitch);
    frame->format(frameFormat_);
    copyLumaData  (stream, pitch, deviceFrame, frame->data());
    copyChromaData(stream, pitch, deviceFrame, frame->data());
    frame->timestamp(static_cast<int64_t>(picture->timestamp));
    CUDA_DRIVER_CALL(cuStreamSynchronize(stream));
    NV_DECODER_CALL(cuvidUnmapVideoFrame(*decoder_, deviceFrame));
  });
  return 1;
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::createDecoder(CUVIDEOFORMAT* format)
{
  const auto capacities = getCapacities(cuda_, format);
  outputFormat_ = pickOutputFormat(format, capacities);
  getFrameMaxSize(format, maxWidth_, maxHeight_);
  calcFrameSize(format);
  format_ = *format;

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
  info.display_area.left   = static_cast<short>(format->display_area.left  );
  info.display_area.top    = static_cast<short>(format->display_area.top   );
  info.display_area.right  = static_cast<short>(format->display_area.right );
  info.display_area.bottom = static_cast<short>(format->display_area.bottom);
  info.ulTargetWidth       = dimWidth_;
  info.ulTargetHeight      = dimHeight_;

  ContextLock lock(cuda_);
  CUvideodecoder decoder{};
  NV_DECODER_CALL(cuvidCreateDecoder(&decoder, &info));
  auto destroyDecoder = [cuda = &cuda_](CUvideodecoder decoder) {
    ContextLock lock(cuda);
    cuvidDestroyDecoder(decoder);
  };
  decoder_ = AutoDestructor<CUvideodecoder>(decoder, destroyDecoder);
  return format->min_num_decode_surfaces;
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::reconfigureDecoder(CUVIDEOFORMAT* format)
{
  if (format->bit_depth_luma_minus8 != format_.bit_depth_luma_minus8 || 
    format->bit_depth_chroma_minus8 != format_.bit_depth_chroma_minus8)
  {
    THROW("Video decoder could not change bit depth");
  }
  if (format->chroma_format != format_.chroma_format) {
    THROW("Video decoder could not change chroma format");
  }
  if ((format->coded_width > maxWidth_ || format->coded_height > maxHeight_) && format->codec != cudaVideoCodec_VP9) {
    THROW("Reconfigure not supported when width/height > maxwidth/maxheight", CUDA_ERROR_NOT_SUPPORTED);
  }
  calcFrameSize(format);
  format_ = *format;
  CUVIDRECONFIGUREDECODERINFO info{};
  info.ulWidth             = format->coded_width;
  info.ulHeight            = format->coded_height;
  info.display_area.bottom = static_cast<short>(format_.display_area.bottom);
  info.display_area.top    = static_cast<short>(format_.display_area.top   );
  info.display_area.left   = static_cast<short>(format_.display_area.left  );
  info.display_area.right  = static_cast<short>(format_.display_area.right );
  info.target_rect.bottom  = static_cast<short>(format_.display_area.bottom);
  info.ulNumDecodeSurfaces = format->min_num_decode_surfaces;
  info.ulTargetWidth       = dimWidth_;
  info.ulTargetHeight      = dimHeight_;
  ContextLock lock(cuda_);
  NV_DECODER_CALL(cuvidReconfigureDecoder(*decoder_, &info));
  return format->min_num_decode_surfaces;
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::calcFrameSize(CUVIDEOFORMAT* format)
{
  frameFormat_ = std::make_shared<Format>();
  Format& frameFormat = *frameFormat_;
  frameFormat.luma.width   = getFrameWidth(outputFormat_, dimWidth_);
  frameFormat.luma.height  = round2(dimHeight_); 
  frameFormat.luma.bytes   =  (format->bit_depth_luma_minus8   + 15) / 8;
  frameFormat.chroma.bytes = ((format->bit_depth_chroma_minus8 + 15) / 8) * 2;
  switch (format->chroma_format) {
  case cudaVideoChromaFormat_420:
    frameFormat.chroma.width  = frameFormat.luma.width  / 2;
    frameFormat.chroma.height = frameFormat.luma.height / 2;
    frameFormat.sampling = Format::Sampling::YUV420;
    break;
  case cudaVideoChromaFormat_422:
    frameFormat.chroma.width  = frameFormat.luma.width / 2;
    frameFormat.chroma.height = frameFormat.luma.height;
    frameFormat.sampling = Format::Sampling::YUV422;
    break;
  case cudaVideoChromaFormat_444:
    frameFormat.chroma.width  = frameFormat.luma.width;
    frameFormat.chroma.height = frameFormat.luma.height;
    frameFormat.sampling = Format::Sampling::YUV444;
    break;
  default:
    frameFormat.chroma.width  = 0;
    frameFormat.chroma.height = 0;
    frameFormat.sampling = Format::Sampling::Invalid;
  }
  frameFormat.luma.size   = frameFormat.luma.width   * frameFormat.luma.height   * frameFormat.luma.bytes;
  frameFormat.chroma.size = frameFormat.chroma.width * frameFormat.chroma.height * frameFormat.chroma.bytes;
  frameFormat.frameSize   = frameFormat.luma.size + frameFormat.chroma.size;
  getYuv2RgbMatrix(format->video_signal_description.matrix_coefficients, frameFormat.colors);
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::copyLumaData(CUstream stream, size_t pitch, CUdeviceptr from, char* to)
{
  CUDA_MEMCPY2D params {};
  params.srcMemoryType = CU_MEMORYTYPE_DEVICE;
  params.dstMemoryType = CU_MEMORYTYPE_HOST;
  params.WidthInBytes  = frameFormat_->luma.width * frameFormat_->luma.bytes;
  params.Height        = frameFormat_->luma.height;
  params.srcPitch      = pitch;
  params.dstPitch      = params.WidthInBytes;
  params.srcDevice     = from;
  params.dstHost       = to;
  params.dstDevice     = (CUdeviceptr)to;
  CUDA_DRIVER_CALL(cuMemcpy2DAsync(&params, stream));
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::copyChromaData(CUstream stream, size_t pitch, CUdeviceptr from, char* to)
{
  const size_t srcOffset = pitch * frameFormat_->luma.height;
  CUDA_MEMCPY2D params {};
  params.srcMemoryType = CU_MEMORYTYPE_DEVICE;
  params.dstMemoryType = CU_MEMORYTYPE_HOST;
  params.WidthInBytes  = frameFormat_->chroma.width * frameFormat_->chroma.bytes;
  params.Height        = frameFormat_->chroma.height;
  params.srcPitch      = pitch;
  params.dstPitch      = params.WidthInBytes;
  params.srcDevice     = (CUdeviceptr)((char*)from + srcOffset);
  params.dstHost       = to + frameFormat_->luma.size;
  params.dstDevice     = (CUdeviceptr)params.dstHost;
  CUDA_DRIVER_CALL(cuMemcpy2DAsync(&params, stream));
}

} // !namespace yaga
