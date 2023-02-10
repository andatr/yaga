#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_MIDDLEWARE
#define YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_MIDDLEWARE

#include <functional>
#include <memory>
#include <string>

#include "frame.h"

namespace yaga {

class DecoderMiddleware
{
public:
  typedef std::function<void(Frame* frame)> Handler;

public:
  virtual ~DecoderMiddleware() {}
  virtual void resize(uint32_t width, uint32_t height) = 0;
  virtual void waitFrame(Handler handler) = 0;
  virtual void getFrame(Handler handler) = 0;
};

typedef std::shared_ptr<DecoderMiddleware> DecoderMiddlewarePtr;

} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_DECODER_MIDDLEWARE
