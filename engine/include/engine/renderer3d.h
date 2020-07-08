#ifndef YAGA_ENGINE_RENDERER_3D
#define YAGA_ENGINE_RENDERER_3D

#include <memory>

#include "engine/component.h"
#include "engine/material.h"
#include "engine/mesh.h"
#include "engine/transform.h"

namespace yaga {

class Renderer3D : public Component
{
public:
  explicit Renderer3D(Object* obj);
  virtual ~Renderer3D() {}
};

typedef std::unique_ptr<Renderer3D> Renderer3DPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_RENDERER_3D
