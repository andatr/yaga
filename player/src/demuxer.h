#ifndef YAGA_PLAYER_SRC_DEMUXER
#define YAGA_PLAYER_SRC_DEMUXER

#include <string>
#include <memory>

#include "codec.h"

namespace yaga {
namespace player {

class Demuxer
{
public:
  virtual ~Demuxer() = 0 {}
  virtual bool demux(uint8_t*& data, uint64_t& size, int64_t& time) = 0;
  virtual CodecId codec() = 0;
};

typedef std::unique_ptr<Demuxer> DemuxerPtr;

DemuxerPtr createDemuxer(const std::string& filename);

} // !namespace player
} // !namespace yaga

#endif // !YAGA_PLAYER_SRC_DEMUXER
