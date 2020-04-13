#include "precompiled.h"
#include "scene_clipper.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
SceneClipper::SceneClipper(Scene* scene) :
  scene_(scene)
{
}

// -------------------------------------------------------------------------------------------------------------------------
const std::vector<Object*>& SceneClipper::process()
{
  cache_.clear();
  cache_.push_back(scene_->root());
  for (const auto& c: scene_->root()->children()) {
    cache_.push_back(c);
  }
  return cache_;
}

} // !namespace yaga

