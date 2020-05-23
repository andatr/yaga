#ifndef YAGA_ENGINE_RENDERING_CONTEXT
#define YAGA_ENGINE_RENDERING_CONTEXT

#include <boost/noncopyable.hpp>
#include <glm/glm.hpp>

#include "camera.h"
#include "material.h"
#include "mesh.h"
#include "renderer3d.h"
#include "transform.h"

namespace yaga
{

class RenderingContext : private boost::noncopyable
{
public:
  virtual ~RenderingContext() {}
  virtual Renderer3DPtr createRenderer3D(Object* object) = 0;
  virtual MaterialPtr createMaterial(Object* object, asset::Material* asset) = 0;
  virtual MeshPtr createMesh(Object* object, asset::Mesh* asset) = 0;
  virtual CameraPtr createCamera(Object* object, asset::Camera* asset) = 0;
  virtual TransformPtr createTransform(Object* object);
  virtual void clear() = 0;
  virtual glm::uvec2 resolution() = 0;
};

typedef std::unique_ptr<RenderingContext> RenderingContextPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_RENDERING_CONTEXT
