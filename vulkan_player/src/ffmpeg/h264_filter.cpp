#include "precompiled.h"
#include "h264_filter.h"

namespace yaga {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
void destroyFilter(AVBSFContext* ctx)
{
  av_bsf_free(&ctx);
  LOG(trace) << "H264 filter destroyed";
}

// -----------------------------------------------------------------------------------------------------------------------------
AVBSFContext* allocateFilter(const char* codecName)
{
  AVBSFContext* context{};
  const AVBitStreamFilter* filter = av_bsf_get_by_name(codecName);
  if (!filter) THROW("Could not get %1% filter", codecName);
  FF_GUARD(av_bsf_alloc(filter, &context), "Could not allocate h264 filter");
  return context;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
H264Filter::H264Filter(const char* codecName, AVCodecParameters* codecParams) :
  bitstreamFilter_(allocateFilter(codecName), destroyFilter)
{
  auto filter = *bitstreamFilter_;
  avcodec_parameters_copy(filter->par_in, codecParams);
  FF_GUARD(av_bsf_init(filter), "Could not initialize h264 filter");
}

// -----------------------------------------------------------------------------------------------------------------------------
void H264Filter::process(AVPacket* packet, uint64_t, uint8_t** data, uint64_t* size, int64_t* time)
{
  auto fpacket = *filteredPacket_;
  FF_GUARD(av_bsf_send_packet   (*bitstreamFilter_,  packet), "av_bsf_send_packet"   );
  FF_GUARD(av_bsf_receive_packet(*bitstreamFilter_, fpacket), "av_bsf_receive_packet");
  *data = fpacket->data;
  *size = fpacket->size;
  *time = fpacket->pts;
}

} // !namespace yaga
