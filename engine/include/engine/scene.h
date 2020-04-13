#ifndef YAGA_ENGINE_SCENE
#define YAGA_ENGINE_SCENE

#include <memory>

#include <boost/core/noncopyable.hpp>

#include "engine/camera.h"

namespace yaga
{
 
class Scene : private boost::noncopyable
{
public:
  explicit Scene();
  virtual ~Scene() {}
  Object* root() const { return root_; }
  Camera* camera() const { return camera_; }
public:
  Object* root_;
  Camera* camera_;
};

typedef std::unique_ptr<Scene> ScenePtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_SCENE
