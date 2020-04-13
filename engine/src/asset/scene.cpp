#include "precompiled.h"
#include "asset/scene.h"
#include "utility/array.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Scene::serializationInfo = {
  8,
  { "yscn" },
  "Scene",
  &Scene::deserialize,
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
void Scene::resolveRefs(Database* db)
{
  model_ = db->get<Model>(modelName_);
}

// -------------------------------------------------------------------------------------------------------------------------
ScenePtr Scene::deserialize(const std::string& name, std::istream& stream, size_t size)
{
  return deserializeFriendly("", name, stream, size);
}

// -------------------------------------------------------------------------------------------------------------------------
ScenePtr Scene::deserializeFriendly(const std::string&, const std::string& name, std::istream&, size_t)
{
  auto scene = std::make_unique<Scene>(name);
  scene->modelName_ = "model";
  return scene;
}

} // !namespace asset
} // !namespace yaga
