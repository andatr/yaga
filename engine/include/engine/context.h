#ifndef YAGA_ENGINE_CONTEXT
#define YAGA_ENGINE_CONTEXT

#include <functional>
#include <boost/noncopyable.hpp>

#include "engine/camera.h"
#include "engine/glm.h"
#include "engine/material.h"
#include "engine/mesh.h"
#include "engine/renderer3d.h"
#include "engine/transform.h"

namespace yaga {

class Context : private boost::noncopyable
{
public:
  virtual ~Context() {}
  virtual Renderer3DPtr createRenderer3D(Object* object) = 0;
  virtual CameraPtr     createCamera    (Object* object);
  virtual TransformPtr  createTransform (Object* object);
  virtual MaterialPtr   createMaterial  (Object* object, assets::Material* asset) = 0;
  virtual MeshPtr       createMesh      (Object* object, assets::Mesh*     asset) = 0;
  virtual float         delta()      const = 0;
  virtual glm::uvec2    resolution() const = 0;
  virtual void          clear() = 0;
  Camera* mainCamera() const { return mainCamera_; }
  void mainCamera(Camera* camera) { mainCamera_ = camera; }

protected:
  Camera* mainCamera_;
};

typedef std::unique_ptr<Context> ContextPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_CONTEXT
