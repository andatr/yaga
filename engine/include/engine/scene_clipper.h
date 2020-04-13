#ifndef YAGA_ENGINE_SCENE_CLIPPER
#define YAGA_ENGINE_SCENE_CLIPPER

#include <memory>

#include <boost/core/noncopyable.hpp>

#include "engine/scene.h"

namespace yaga
{

class SceneClipper : private boost::noncopyable
{
public:
  explicit SceneClipper(Scene* scene);
  virtual ~SceneClipper() {}
  virtual const std::vector<Object*>& process();
private:
  Scene* scene_;
  std::vector<Object*> cache_;
};

typedef std::unique_ptr<SceneClipper> SceneClipperPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_SCENE_CLIPPER
