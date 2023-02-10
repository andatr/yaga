#include "precompiled.h"
#include "h264_filter.h"

namespace yaga {
namespace player {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
AutoDestructor<AVBSFContext*> allocateFilter(const char* name)
{
  AVBSFContext* context{};
  const AVBitStreamFilter* filter = av_bsf_get_by_name(name);
  if (!filter) THROW("Could not get %1% filter", name);
  FF_GUARD(av_bsf_alloc(filter, &context), "Could not allocate h264 filter");
  auto destroyContext = [](AVBSFContext* ctx) {
    av_bsf_free(&ctx);
    LOG(trace) << "H264 filter destroyed";
  };
  return AutoDestructor<AVBSFContext*>(context, destroyContext);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
H264Filter::H264Filter(const char* name, AVCodecParameters* codecParams) :
  filteredPacket_{},
  bitstreamFilter_(allocateFilter(name))
{
  auto filter = *bitstreamFilter_;
  avcodec_parameters_copy(filter->par_in, codecParams);
  FF_GUARD(av_bsf_init(filter), "Could not initialize h264 filter");

  av_init_packet(&filteredPacket_);
  filteredPacket_.data = nullptr;
  filteredPacket_.size = 0;
}

// -----------------------------------------------------------------------------------------------------------------------------
H264Filter::~H264Filter()
{
  if (filteredPacket_.data) {
    av_packet_unref(&filteredPacket_);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void H264Filter::Demux(AVPacket& packet, uint64_t, uint8_t*& data, uint64_t& size, int64_t& time)
{
  if (filteredPacket_.data) {
    av_packet_unref(&filteredPacket_);
  }
  FF_GUARD(av_bsf_send_packet   (*bitstreamFilter_, &packet         ), "av_bsf_send_packet"   );
  FF_GUARD(av_bsf_receive_packet(*bitstreamFilter_, &filteredPacket_), "av_bsf_receive_packet");
  data = filteredPacket_.data;
  size = filteredPacket_.size;
  time = filteredPacket_.pts;
}

} // !namespace player
} // !namespace yaga
