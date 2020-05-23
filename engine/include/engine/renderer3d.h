#ifndef YAGA_ENGINE_RENDERER_3D
#define YAGA_ENGINE_RENDERER_3D

#include <memory>

#include "component.h"
#include "material.h"
#include "mesh.h"
#include "transform.h"

namespace yaga
{

class Renderer3D : public Component
{
public:
  explicit Renderer3D(Object* obj);
  virtual ~Renderer3D() {}
};

typedef std::unique_ptr<Renderer3D> Renderer3DPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_RENDERER_3D
