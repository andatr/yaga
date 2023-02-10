#ifndef YAGA_ENGINE_RENDERER
#define YAGA_ENGINE_RENDERER

#include <memory>
#include <cstdint>

#include "engine/component.h"

namespace yaga {

class Renderer : public Component
{
public:
  struct PropertyIndex
  {
    static const int layer  = 0;
  };

public:
  explicit Renderer();
  virtual ~Renderer();
  uint32_t layer() const { return layer_; }
  Renderer* layer(uint32_t value);

protected:
  uint32_t layer_;
};

typedef std::unique_ptr<Renderer> RendererPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_RENDERER
