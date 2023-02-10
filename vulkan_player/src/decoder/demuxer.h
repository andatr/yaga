#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_DEMUXER
#define YAGA_VULKAN_PLAYER_SRC_DECODER_DEMUXER

#include "utility/compiler.h"

#include <string>
#include <memory>
#include <vector>

DISABLE_WARNINGS
#include <libavcodec/codec_id.h>
ENABLE_WARNINGS

#include "player.h"

namespace yaga {

class DemuxerVisitor
{
public:
  virtual ~DemuxerVisitor() {}
  virtual void processVideoFrame    (uint8_t* data, uint64_t size, int64_t time) = 0;
  virtual void processAudioFrame    (uint8_t* data, uint64_t size, int64_t time) = 0;
  virtual void processSubtitlesFrame(uint8_t* data, uint64_t size, int64_t time) = 0;
};

class DemuxerStream : public Stream
{
public:
  virtual AVCodecID codec() = 0;
};

class DemuxerStreamFamily : public StreamFamily
{
public:
  virtual DemuxerStream* stream(int index) = 0;
};

class Demuxer
{
public:
  virtual ~Demuxer()                       = 0 {}
  virtual bool readFrame()                 = 0;
  virtual DemuxerStreamFamily* videoStreams()    = 0;
  virtual DemuxerStreamFamily* audioStreams()    = 0;
  virtual DemuxerStreamFamily* subtitleStreams() = 0;
  virtual int64_t duration()               = 0;
  virtual void seek(int64_t timestamp)     = 0;
  virtual const Description& description() = 0;
  virtual const std::string& title()       = 0;
  virtual const std::vector<Chapter>& chapters() = 0;
};

typedef std::unique_ptr<Demuxer> DemuxerPtr;

DemuxerPtr createDemuxer(const std::string& filename, DemuxerVisitor* visitor);

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_DEMUXER
