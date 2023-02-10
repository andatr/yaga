#ifndef YAGA_ENGINE_RENDERER_3D
#define YAGA_ENGINE_RENDERER_3D

#include <memory>

#include "engine/renderer.h"

namespace yaga {

class Renderer3D : public Renderer
{
public:
  explicit Renderer3D();
  virtual ~Renderer3D() {}
  const std::string& name() override;
};

typedef std::unique_ptr<Renderer3D> Renderer3DPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_RENDERER_3D
