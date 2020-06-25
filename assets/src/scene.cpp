#include "precompiled.h"
#include "assets/scene.h"
#include "utility/array.h"

namespace yaga
{
namespace assets
{

const SerializationInfo Scene::serializationInfo = {
  (uint32_t)StandardAssetId::scene,
  { "yscn" },
  &Scene::deserializeBinary,
  &Scene::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Scene::Scene(const std::string& name) :
  Asset(name), model_(nullptr)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Scene::~Scene()
{
}

// -------------------------------------------------------------------------------------------------------------------------
ScenePtr Scene::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -------------------------------------------------------------------------------------------------------------------------
ScenePtr Scene::deserializeFriendly(const std::string& name, const std::string&, RefResolver& resolver)
{
  auto scene = std::make_unique<Scene>(name);
  scene->model_ = resolver.getAsset<Model>("model");
  return scene;
}

} // !namespace assets
} // !namespace yaga
