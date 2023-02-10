#include "precompiled.h"
#include "ff_decoder.h"

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<AVFormatContext*> createContext(const std::string& filename)
{
  AVFormatContext* ctx = nullptr;
  FF_GUARD(avformat_open_input(&ctx, filename.c_str(), NULL, NULL), "Could not open file", filename);
  auto destroyContext = [](AVFormatContext* ctx) {
    avformat_close_input(&ctx);
    LOG(trace) << "FFmpeg: format context destroyed";
  };
  FF_GUARD(avformat_find_stream_info(ctx, NULL), "Could not find stream information");
  return AutoDestructor<AVFormatContext*>(ctx, destroyContext);
}

// -----------------------------------------------------------------------------------------------------------------------------
void destroyContext(AVCodecContext* ctx)
{
  avcodec_free_context(&ctx);
  LOG(trace) << "FFmpeg: codec context destroyed";
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Codec::Codec(AVFormatContext* formatContext, AVMediaType type) :
  streamIndex_(-1),
  frame_(av_frame_alloc(), [](AVFrame* frame) { av_frame_free(&frame); })
{
  streamIndex_ = FF_GUARD(av_find_best_stream(formatContext, type, -1, -1, NULL, 0),
    "Could not find stream %2%", type);
  AVStream* stream = formatContext->streams[streamIndex_];
  const AVCodec* decoder = FF_GUARD_PTR(avcodec_find_decoder(stream->codecpar->codec_id),
    "Could not find %1% codec", type);
  AVCodecContext* decoderContext = FF_GUARD_PTR(avcodec_alloc_context3(decoder),
    "Could not allocate the %1% codec context", type);
  FF_GUARD(avcodec_parameters_to_context(decoderContext, stream->codecpar),
    "Could not copy %2% codec parameters to decoder context", type);
  FF_GUARD(avcodec_open2(decoderContext, decoder, NULL),
    "Could not open %2% codec", type);
  context_.set(decoderContext, destroyContext);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Codec::sendPacket(AVPacket* packet)
{
  if (packet->stream_index != streamIndex_) return false;
  FF_GUARD_AV(avcodec_send_packet(*context_, packet), "Could not submit a packet for decoding");
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Codec::getFrame()
{
  int code = avcodec_receive_frame(*context_, *frame_);
  if (code < 0) {
    if (code == AVERROR_EOF || code == AVERROR(EAGAIN)) return false;
    THROW("FFmpeg: Error during decoding: %1%", avErr2Str(code));
  }
  // process frame
  av_frame_unref(*frame_);
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
FFDecoder::FFDecoder(const std::string& filename) :
  eof_(false),
  formatContext_(createContext(filename)),
  packet_(av_packet_alloc(), [](AVPacket* packet) { av_packet_free(&packet); })
{
  LOG(debug) << "Media format: " 
    << (*formatContext_)->iformat->long_name
    << " (" << (*formatContext_)->iformat->name << ")";
  codecs_.emplace_back(std::make_unique<Codec>(*formatContext_, AVMEDIA_TYPE_VIDEO));
  //codecs_.emplace_back(std::make_unique<Codec>(*formatContext_, AVMEDIA_TYPE_AUDIO));
}

// ------------------------------------------------------------------------------------------------------------------------------
FFDecoder::~FFDecoder()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
bool FFDecoder::decode()
{
  while(!eof_) {
    for (auto& codec: codecs_) {
      if (codec->getFrame()) return true;
    }
    if (av_read_frame(*formatContext_, *packet_) < 0) {
      eof_ = true;
      return false;
    }
    for (auto& codec: codecs_) {
      if (codec->sendPacket(*packet_)) break;
    }
    av_packet_unref(*packet_);
  }
  return false;
}

} // !namespace yaga
