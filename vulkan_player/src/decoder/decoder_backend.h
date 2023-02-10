#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_BACKEND
#define YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_BACKEND

#include <functional>
#include <memory>
#include <vector>

#include "utility/compiler.h"

DISABLE_WARNINGS
#include <libavcodec/codec_id.h>
ENABLE_WARNINGS

#include "frame.h"

namespace yaga {

class DecoderBackend
{
public:
  typedef std::function<void(std::function<void(Frame* frame)>)> Handler;

public:
  virtual ~DecoderBackend() {}
  virtual void decode(const uint8_t* data, uint64_t size, int flags, int64_t timestamp) = 0;
  virtual void resize(uint32_t width, uint32_t height) = 0;
};

typedef std::unique_ptr<DecoderBackend> DecoderBackendPtr;

DecoderBackendPtr createDecoderBackend(
  AVCodecID codec,
  unsigned clockRate,
  uint32_t width,
  uint32_t height,
  DecoderBackend::Handler handler
);

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_BACKEND
