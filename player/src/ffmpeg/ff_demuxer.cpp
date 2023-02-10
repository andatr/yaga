#include "precompiled.h"
#include "ff_demuxer.h"

namespace yaga {
namespace player {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<AVFormatContext*> createContext(const std::string& filename)
{
  AVFormatContext* ctx = nullptr;
  FF_GUARD(avformat_open_input(&ctx, filename.c_str(), NULL, NULL), "Could not create FFmpeg context");
  auto destroyContext = [](AVFormatContext* ctx) {
    avformat_close_input(&ctx);
    LOG(trace) << "FFmpeg context destroyed";
  };
  return AutoDestructor<AVFormatContext*>(ctx, destroyContext);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
FFDemuxer::InitSsl::InitSsl()
{
  avformat_network_init();
}

// -----------------------------------------------------------------------------------------------------------------------------
FFDemuxer::InitSsl::~InitSsl()
{
  avformat_network_deinit();
}

// -----------------------------------------------------------------------------------------------------------------------------
DemuxerPtr createDemuxer(const std::string& filename)
{
  return std::make_unique<FFDemuxer>(filename);
}

// -----------------------------------------------------------------------------------------------------------------------------
FFDemuxer::FFDemuxer(const std::string& filename, int64_t timeScaleHz) :
  context_(createContext(filename)),
  videoStream_(-1),
  videoCodec_(AV_CODEC_ID_NONE),
  width_(0),
  height_(0),
  pixelFormat_(AV_PIX_FMT_NONE),
  bitDepth_(0),
  bitsPerPixel_(0),
  chromaHeight_(0),
  timeBase_(0.0),
  timeScale_(timeScaleHz),
  packet_{},
  frameCount_(0)
{
  if (context_.destoyed()) return;
  AVFormatContext* context = *context_;
  LOG(debug) << "Media format: " << context->iformat->long_name << " (" << context->iformat->name << ")";
  FF_GUARD(avformat_find_stream_info(context, NULL),
    "FFmpeg could not find stream info");
  videoStream_ = FF_GUARD(av_find_best_stream(context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0),
    "FFmpeg could not find video stream");
  getPixelFormat();
  getVideoFilter();
  getTimeScale();
  initPacket();
}

// ------------------------------------------------------------------------------------------------------------------------------
FFDemuxer::~FFDemuxer()
{
  if (packet_.data) {
    av_packet_unref(&packet_);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFDemuxer::getPixelFormat()
{
  AVFormatContext* context = *context_;
  width_  = context->streams[videoStream_]->codecpar->width;
  height_ = context->streams[videoStream_]->codecpar->height;
  pixelFormat_ = (AVPixelFormat)context->streams[videoStream_]->codecpar->format;
  switch (pixelFormat_) {
    case AV_PIX_FMT_YUV420P10LE:
    case AV_PIX_FMT_GRAY10LE:   // monochrome is treated as 420 with chroma filled with 0x0
      bitDepth_ = 10;
      chromaHeight_ = (height_ + 1) >> 1;
      bitsPerPixel_ = 2;
      break;
    case AV_PIX_FMT_YUV420P12LE:
      bitDepth_ = 12;
      chromaHeight_ = (height_ + 1) >> 1;
      bitsPerPixel_ = 2;
      break;
    case AV_PIX_FMT_YUV444P10LE:
      bitDepth_ = 10;
      chromaHeight_ = height_ << 1;
      bitsPerPixel_ = 2;
      break;
    case AV_PIX_FMT_YUV444P12LE:
      bitDepth_ = 12;
      chromaHeight_ = height_ << 1;
      bitsPerPixel_ = 2;
      break;
    case AV_PIX_FMT_YUV444P:
      bitDepth_ = 8;
      chromaHeight_ = height_ << 1;
      bitsPerPixel_ = 1;
      break;
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUVJ420P:
    case AV_PIX_FMT_YUVJ422P:   // jpeg decoder output is subsampled to NV12 for 422/444 so treat it as 420
    case AV_PIX_FMT_YUVJ444P:   // jpeg decoder output is subsampled to NV12 for 422/444 so treat it as 420
    case AV_PIX_FMT_GRAY8:      // monochrome is treated as 420 with chroma filled with 0x0
      bitDepth_ = 8;
      chromaHeight_ = (height_ + 1) >> 1;
      bitsPerPixel_ = 1;
      break;
    default:
      LOG(warning) << "FFmpeg Pixel Format " << pixelFormat_ << " not recognized, assuming 420";
      pixelFormat_ = AV_PIX_FMT_YUV420P;
      bitDepth_ = 8;
      chromaHeight_ = (height_ + 1) >> 1;
      bitsPerPixel_ = 1;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFDemuxer::getVideoFilter()
{
  AVFormatContext* context = *context_;
  videoCodec_ = context->streams[videoStream_]->codecpar->codec_id;
  const char* codecName = context->iformat->long_name;
  auto codecParams = context->streams[videoStream_]->codecpar;
  videoFilter_ = chooseVideoFilter(videoCodec_, codecName, codecParams);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFDemuxer::getTimeScale()
{
  auto timeBase = (*context_)->streams[videoStream_]->time_base;
  timeBase_ = av_q2d(timeBase);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFDemuxer::initPacket()
{
  av_init_packet(&packet_);
  packet_.data = nullptr;
  packet_.size = 0;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool FFDemuxer::demux(uint8_t*& data, uint64_t& size, int64_t& time)
{
  if (context_.destoyed()) return false;
  AVFormatContext* context = *context_;
  size = 0;
  if (packet_.data) {
    av_packet_unref(&packet_);
  }
  int error = 0;
  while ((error = av_read_frame(context, &packet_)) >= 0 && packet_.stream_index != videoStream_) {
    av_packet_unref(&packet_);
  }
  if (error < 0) return false;
  videoFilter_->Demux(packet_, frameCount_, data, size, time);
  time = static_cast<int64_t>(time * timeScale_ * timeBase_);
  ++frameCount_;
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
CodecId FFDemuxer::codec()
{ 
  switch (videoCodec_) {
    case AV_CODEC_ID_MPEG1VIDEO: return CodecId::MPEG1; 
    case AV_CODEC_ID_MPEG2VIDEO: return CodecId::MPEG2; 
    case AV_CODEC_ID_MPEG4:      return CodecId::MPEG4; 
    case AV_CODEC_ID_WMV3:       return CodecId::WMV3; 
    case AV_CODEC_ID_VC1:        return CodecId::VC1; 
    case AV_CODEC_ID_H264:       return CodecId::H264; 
    case AV_CODEC_ID_HEVC:       return CodecId::HEVC; 
    case AV_CODEC_ID_VP8:        return CodecId::VP8; 
    case AV_CODEC_ID_VP9:        return CodecId::VP9; 
    case AV_CODEC_ID_MJPEG:      return CodecId::MJPEG; 
    case AV_CODEC_ID_AV1:        return CodecId::AV1;
    default:                     return CodecId::None;
  }
}

} // !namespace player
} // !namespace yaga
