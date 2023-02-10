#include "precompiled.h"
#include "mp4_filter.h"

namespace yaga {
namespace player {

// -----------------------------------------------------------------------------------------------------------------------------
Mp4Filter::Mp4Filter(AVCodecParameters* codecParams) :
  filteredPacket_(nullptr),
  extraData_(codecParams->extradata),
  extraSize_(codecParams->extradata_size)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Mp4Filter::~Mp4Filter()
{
  if (filteredPacket_) {
    av_free(filteredPacket_);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Mp4Filter::Demux(AVPacket& packet, uint64_t frame, uint8_t*& data, uint64_t& size, int64_t& time)
{
  if (frame == 0) {
    if (extraSize_ > 0) {
      constexpr size_t offset = 3 * sizeof(uint8_t); // extra data contains start codes 00 00 01
      uint64_t extraSize = extraSize_ + packet.size - offset;
      filteredPacket_ = (uint8_t *)av_malloc(size);
      if (!filteredPacket_) THROW("Could not allocate extra %1% bytes for Mp4 decoder", size);
      memcpy(filteredPacket_, extraData_, extraSize_);
      memcpy(filteredPacket_ + extraSize_, packet.data + offset, packet.size - offset);
      data = filteredPacket_;
      size = extraSize;
    }
  }
  else {
    data = packet.data;
    size = packet.size;
  }
  time = packet.pts;
}

} // !namespace player
} // !namespace yaga
