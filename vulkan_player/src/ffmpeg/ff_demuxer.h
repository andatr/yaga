#ifndef YAGA_VULKAN_PLAYER_SRC_FFMPEG_DEMUXER
#define YAGA_VULKAN_PLAYER_SRC_FFMPEG_DEMUXER

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ffmpeg.h"
#include "decoder/demuxer.h"
#include "video_filter.h"
#include "utility/auto_destructor.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class FFStream : public DemuxerStream
{
public:
  explicit FFStream(AVStream* stream);
  AVCodecID codec()  override { return stream_->codecpar->codec_id; } 
  const std::string& name()        override { return name_;         }
  const Description& description() override { return description_;  }
  AVStream* avstream() { return stream_; }

private:
  AVStream* stream_;
  std::string name_;
  Description description_;
};

typedef std::unique_ptr<FFStream> FFStreamPtr;

// -----------------------------------------------------------------------------------------------------------------------------
class FFStreamFamily : public DemuxerStreamFamily
{
friend class FFDemuxer;
public:
  typedef std::function<void(uint8_t*, uint64_t, int64_t)> Proc;

public:
  FFStreamFamily(AVFormatContext* context, AVMediaType type, Proc proc);
  ~FFStreamFamily() {}
  int count()                      override;
  int current()                    override;
  DemuxerStream* stream(int index) override;
  void activateStream(int index)   override;

protected:
  virtual bool process(AVPacket* packet);

protected:
  int current_;
  Proc proc_;
  std::vector<FFStreamPtr> streams_;
};

typedef std::unique_ptr<FFStreamFamily> FFStreamFamilyPtr;

// -----------------------------------------------------------------------------------------------------------------------------
class VideoStreamFamily : public FFStreamFamily
{
public:
  VideoStreamFamily(AVFormatContext* context, Proc proc);
  ~VideoStreamFamily() {}
  void activateStream(int index) override;

protected:
  bool process(AVPacket* packet) override;

private:
  const char* codecName;
  uint64_t frameCount_;
  AVRational timeBase_;
  VideoFilterPtr videoFilter_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class FFDemuxer : public Demuxer
{
public:
  FFDemuxer(const std::string& filename, DemuxerVisitor* visitor);
  bool readFrame() override;
  DemuxerStreamFamily* videoStreams()    override;
  DemuxerStreamFamily* audioStreams()    override;
  DemuxerStreamFamily* subtitleStreams() override;
  void seek(int64_t timestamp)     override;
  const Description& description() override { return description_;    }
  const std::string& title()       override { return title_;          }
  int64_t duration()               override { return duration_;       }
  const std::vector<Chapter>& chapters() override { return chapters_; }

private:
  class InitSsl
  {
  public:
    InitSsl();
    ~InitSsl();
  };

private:
  void makeTitle();
  void makeChapters();

private:
  static InitSsl initSsl_;
  AutoDestructor<AVFormatContext*> context_;
  VideoFilterPtr videoFilter_;
  Packet packet_;
  FFStreamFamilyPtr videoStreams_;
  FFStreamFamilyPtr audioStreams_;
  FFStreamFamilyPtr subtitleStreams_;
  int64_t duration_;
  std::string title_;
  Description description_;
  std::vector<Chapter> chapters_;
};

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_FFMPEG_DEMUXER
