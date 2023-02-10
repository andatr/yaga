#include "precompiled.h"
#include "ff_demuxer.h"
#include "ff_print.h"

#define BIND(proc) std::bind(&DemuxerVisitor::proc, visitor, ph::_1, ph::_2, ph::_3)

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<AVFormatContext*> createContext(const std::string& filename)
{
  AVFormatContext* ctx = nullptr;
  FF_GUARD(avformat_open_input(&ctx, filename.c_str(), nullptr, nullptr), "Could not create FFmpeg context");
  auto destroyContext = [](AVFormatContext* ctx) {
    avformat_close_input(&ctx);
    LOG(trace) << "FFmpeg context destroyed";
  };
  return AutoDestructor<AVFormatContext*>(ctx, destroyContext);
}

} // !namespace

namespace ph = std::placeholders;

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
DemuxerPtr createDemuxer(const std::string& filename, DemuxerVisitor* visitor)
{
  return std::make_unique<FFDemuxer>(filename, visitor);
}

// -----------------------------------------------------------------------------------------------------------------------------
Packet::Packet() :
  packet_(av_packet_alloc())
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Packet::~Packet()
{
  if (packet_->data) {
    av_packet_unref(packet_);
  }
  destroyPacket(packet_);
}

// -----------------------------------------------------------------------------------------------------------------------------
AVPacket* Packet::operator*()
{
  if (packet_->data) {
    av_packet_unref(packet_);
  }
  return packet_;
}

// -----------------------------------------------------------------------------------------------------------------------------
FFStream::FFStream(AVStream* stream) :
  stream_(stream),
  name_       (makeStreamName       (stream)),
  description_(makeStreamDescription(stream))
{
}

// -----------------------------------------------------------------------------------------------------------------------------
FFStreamFamily::FFStreamFamily(AVFormatContext* context, AVMediaType type, Proc proc) :
  current_(-1),
  proc_(proc)
{
  auto best = av_find_best_stream(context, type, -1, -1, nullptr, 0);
  const int n = static_cast<int>(context->nb_streams);
  for (int i = 0; i < n; ++i) {
    if (context->streams[i]->codecpar->codec_type == type) {
      streams_.push_back(std::make_unique<FFStream>(context->streams[i]));
      if (i == best) {
        current_ = static_cast<int>(streams_.size()) - 1;
      }
    }
  }
  if (current_ == -1 && !streams_.empty()) {
    current_ = 0;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
int FFStreamFamily::count()
{
  return static_cast<int>(streams_.size());
}

// -----------------------------------------------------------------------------------------------------------------------------
int FFStreamFamily::current()
{
  return current_;
}

// -----------------------------------------------------------------------------------------------------------------------------
DemuxerStream* FFStreamFamily::stream(int index)
{
  if (index < 0 || index >= static_cast<int>(streams_.size())) THROW("Wrong index");
  return streams_[index].get();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFStreamFamily::activateStream(int index)
{
  if (index < 0 || index >= static_cast<int>(streams_.size())) THROW("Wrong index");
  current_ = index;
}

// -----------------------------------------------------------------------------------------------------------------------------
bool FFStreamFamily::process(AVPacket* packet)
{
  if (current_ > -1 && packet->stream_index != streams_[current_]->avstream()->index) return false;
  proc_(packet->data, packet->size, packet->pts);
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
VideoStreamFamily::VideoStreamFamily(AVFormatContext* context, Proc proc) :
  FFStreamFamily(context, AVMEDIA_TYPE_VIDEO, proc),
  frameCount_(0),
  timeBase_{},
  codecName(context->iformat->long_name)
{
  if (current_ > -1) {
    videoFilter_ = chooseVideoFilter(codecName, streams_[current_]->avstream()->codecpar);
    timeBase_ = streams_[current_]->avstream()->time_base;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void VideoStreamFamily::activateStream(int index)
{
  FFStreamFamily::activateStream(index);
  if (current_ > -1) {
    videoFilter_ = chooseVideoFilter(codecName, streams_[current_]->avstream()->codecpar);
    timeBase_ = streams_[current_]->avstream()->time_base;
  } 
}

// -----------------------------------------------------------------------------------------------------------------------------
bool VideoStreamFamily::process(AVPacket* packet)
{
  if (current_ > -1 && packet->stream_index != streams_[current_]->avstream()->index) return false;
  uint8_t* data = nullptr;
  uint64_t size = 0;
  int64_t  time = 0;
  videoFilter_->process(packet, frameCount_++, &data, &size, &time);
  time = static_cast<int64_t>(static_cast<double>(time) * timeBase_.num / timeBase_.den);
  proc_(data, size, time);
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
FFDemuxer::FFDemuxer(const std::string& filename, DemuxerVisitor* visitor) :
  context_(createContext(filename)),
  duration_(0)
{
  if (context_.destoyed()) return;
  FF_GUARD(avformat_find_stream_info(*context_, nullptr), "Could not find stream info");
  videoStreams_    = std::make_unique<VideoStreamFamily>(*context_,                        BIND(processVideoFrame    ));
  audioStreams_    = std::make_unique<FFStreamFamily>   (*context_, AVMEDIA_TYPE_AUDIO,    BIND(processAudioFrame    ));
  subtitleStreams_ = std::make_unique<FFStreamFamily>   (*context_, AVMEDIA_TYPE_SUBTITLE, BIND(processSubtitlesFrame));
  duration_        = getDuration(*context_);
  title_           = getTitle(*context_);
  description_     = makeFormatDescription(*context_);
  makeChapters();
  seek(1000000000);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool FFDemuxer::readFrame()
{
  if (context_.destoyed()) return false;
  AVFormatContext* context = *context_;
  AVPacket* pkt  = nullptr;
  while (true) {
    pkt = *packet_;
    if (av_read_frame(context, pkt) < 0) return false;
    if (videoStreams_   ->process(pkt)) return true;
    if (audioStreams_   ->process(pkt)) return true;
    if (subtitleStreams_->process(pkt)) return true;
  }
  return false;
}

// -----------------------------------------------------------------------------------------------------------------------------
DemuxerStreamFamily* FFDemuxer::videoStreams()
{
  return videoStreams_.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
DemuxerStreamFamily* FFDemuxer::audioStreams()
{
  return audioStreams_.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
DemuxerStreamFamily* FFDemuxer::subtitleStreams()
{
  return subtitleStreams_.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFDemuxer::seek(int64_t timestamp)
{
  if (context_.destoyed() || videoStreams_->streams_.empty()) return;
  //auto stream = videoStreams_->streams_[videoStreams_->current_]->avstream();
  //timestamp = static_cast<int64_t>(static_cast<double>(timestamp) * stream->time_base.den / stream->time_base.num);
  //av_seek_frame(*context_, stream->index, timestamp, 0);
  av_seek_frame(*context_, -1, timestamp, 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
void FFDemuxer::makeChapters()
{
  chapters_.resize(context_->nb_chapters);
  for (unsigned int i = 0; i < context_->nb_chapters; ++i) {
    const auto chapter = context_->chapters[i];
    const double timestamp = static_cast<double>(chapter->start) * chapter->time_base.num / chapter->time_base.den;
    chapters_[i].timestamp = static_cast<int64_t>(timestamp);
    char* buffer = nullptr;
    av_dict_get_string(chapter->metadata, &buffer, ':', '\n');
    auto title = av_dict_get(chapter->metadata, "title", nullptr, 0);
    if (title) {
      chapters_[i].name = title->value;
    }
    else {
      chapters_[i].name = "Chapter " + std::to_string(i + 1);
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
/*void getPixelFormat()
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
}*/

} // !namespace yaga
