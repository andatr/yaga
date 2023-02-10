#ifndef YAGA_ENGINE_CONTEXT
#define YAGA_ENGINE_CONTEXT

#include "utility/compiler.h"

#include <functional>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "engine/camera.h"
#include "engine/material.h"
#include "engine/mesh.h"
#include "engine/post_processor.h"
#include "engine/renderer3d.h"
#include "engine/transform.h"
#include "utility/glm.h"

namespace yaga {

class Context : private boost::noncopyable
{
public:
  Context();
  virtual ~Context() {}
  virtual Renderer3DPtr createRenderer3D() = 0;
  virtual CameraPtr     createCamera    (assets::CameraPtr    asset) = 0;
  virtual TransformPtr  createTransform (assets::TransformPtr asset);
  virtual MaterialPtr   createMaterial  (assets::MaterialPtr  asset) = 0;
  virtual MeshPtr       createMesh      (assets::MeshPtr      asset) = 0;
  virtual PostProcessorPtr createPostProcessor(int order, bool hostMemory, PostProcessor::Proc proc) = 0;
  virtual void          clear()            = 0;
  virtual float         delta()      const = 0;
  virtual glm::uvec2    resolution() const = 0;
  virtual Camera*       mainCamera() const = 0;
  virtual void          mainCamera(Camera* camera) = 0;
};

typedef std::unique_ptr<Context> ContextPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_CONTEXT
