#include "precompiled.h"
#include "nv_decoder.h"

#define DECODER(data) static_cast<NvDecoder*>(userData)
#define SIZE(h) static_cast<int>(h.size())

namespace yaga {
namespace player {
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
cudaVideoCodec getNvCodecId(CodecId id)
{
  switch (id) {
    case CodecId::MPEG1: return cudaVideoCodec_MPEG1;
    case CodecId::MPEG2: return cudaVideoCodec_MPEG2;
    case CodecId::MPEG4: return cudaVideoCodec_MPEG4;
    case CodecId::WMV3:
    case CodecId::VC1:   return cudaVideoCodec_VC1;
    case CodecId::H264:  return cudaVideoCodec_H264;
    case CodecId::HEVC:  return cudaVideoCodec_HEVC;
    case CodecId::VP8:   return cudaVideoCodec_VP8;
    case CodecId::VP9:   return cudaVideoCodec_VP9;
    case CodecId::MJPEG: return cudaVideoCodec_JPEG;
    case CodecId::AV1:   return cudaVideoCodec_AV1;
    default: THROW("Unknown video codec: %1%", (int)id);
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
DecoderPtr createDecoder(
  CodecId  codec, 
  unsigned clockRate, 
  uint32_t width, 
  uint32_t height, 
  VideoPlayerFrame** frames, 
  int frameCount)
{
  return std::make_unique<NvDecoder>(getNvCodecId(codec), clockRate, width, height, frames, frameCount);
}

// -----------------------------------------------------------------------------------------------------------------------------
NvDecoder::NvDecoder(
  cudaVideoCodec codec,
  unsigned clockRate,
  uint32_t width,
  uint32_t height,
  VideoPlayerFrame** frames,
  int frameCount
) :
  cuda_{},
  operatingPoint_(),
  displayAllLayers_(false),
  deviceFrame_(&cuda_, width, height),
  frames_(frameCount),
  converter_(&cuda_),
  parser_{},
  reconfig_(false)
{
  for (int i = 0; i < frameCount; ++i) {
    frames_[i] = frames[i];
  }
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
    cuvidDestroyVideoParser(p);
  };
  parser_.set(parser, deleteParser);
}

// ------------------------------------------------------------------------------------------------------------------------------
NvDecoder::~NvDecoder()
{
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::decode(const uint8_t* data, int size, int flags, int64_t timestamp)
{
  if (reconfig_) {
    reconfig_ = false;
    deviceFrame_.changeFormat(nullptr);
  }
  CUVIDSOURCEDATAPACKET params{};
  params.payload      = data;
  params.payload_size = size;
  params.flags        = flags | CUVID_PKT_TIMESTAMP;
  params.timestamp    = timestamp;
  if (!data || size == 0) {
    params.flags |= CUVID_PKT_ENDOFSTREAM;
  }
  NV_DECODER_CALL(cuvidParseVideoData(*parser_, &params));
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::getFrame(VideoPlayerFrameProcessor* proc)
{
  auto frame = popFrameRead();
  proc->processFrame(frame);
  pushFrameRead();
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::stop()
{
  std::unique_lock<std::mutex> rlock(readSync_.mutex);
  readSync_.count  = 0;
  rlock.unlock();
  readSync_.condition.notify_one();
  
  std::unique_lock<std::mutex> wlock(writeSync_.mutex);
  writeSync_.count = 0;
  wlock.unlock();
  writeSync_.condition.notify_one();
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::resize(uint32_t width, uint32_t height)
{
  deviceFrame_.resize(width, height);
  reconfig_ = true;
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
  return deviceFrame_.changeFormat(format);
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::decodePicture(CUVIDPICPARAMS* picture)
{
  return deviceFrame_.decode(picture);
}

// ------------------------------------------------------------------------------------------------------------------------------
int NvDecoder::displayPicture(CUVIDPARSERDISPINFO* picture)
{
  auto frame = popFrameWrite();
  deviceFrame_.map(picture, frame);
  pushFrameWrite();
  return 1;
}

// ------------------------------------------------------------------------------------------------------------------------------
VideoPlayerFrame* NvDecoder::popFrameWrite()
{
  std::unique_lock<std::mutex> lock(readSync_.mutex);
  readSync_.condition.wait(lock, [this] { 
    return writeSync_.count >= readSync_.count
      ? writeSync_.count + 1 - readSync_.count != SIZE(frames_)
      : readSync_.count != writeSync_.count + 1;
  });
  return frames_[writeSync_.count];
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::pushFrameWrite()
{
  std::unique_lock<std::mutex> lock(writeSync_.mutex);
  ++writeSync_.count;
  if (writeSync_.count >= SIZE(frames_)) {
    writeSync_.count -= SIZE(frames_);
  }
  lock.unlock();
  writeSync_.condition.notify_one();
}

// ------------------------------------------------------------------------------------------------------------------------------
VideoPlayerFrame* NvDecoder::popFrameRead()
{
  std::unique_lock<std::mutex> lock(writeSync_.mutex);
  writeSync_.condition.wait(lock, [this](){
    return readSync_.count != writeSync_.count;
  });
  return frames_[readSync_.count];
}

// ------------------------------------------------------------------------------------------------------------------------------
void NvDecoder::pushFrameRead()
{
  std::unique_lock<std::mutex> lock(readSync_.mutex);
  ++readSync_.count;
  if (readSync_.count >= SIZE(frames_)) {
    readSync_.count -= SIZE(frames_);
  }
  lock.unlock();
  readSync_.condition.notify_one();
}

} // !namespace player
} // !namespace yaga
