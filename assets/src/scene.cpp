#include "precompiled.h"
#include "assets/scene.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {

BINARY_SERIALIZER_REG(Scene)

// -----------------------------------------------------------------------------------------------------------------------------
Scene::Scene(const std::string& name) :
  Asset(name),
  model_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Scene::~Scene()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ScenePtr Scene::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto scene = std::make_unique<Scene>(name);
  return scene;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Scene::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto scene = assetCast<Scene>(asset);
  binser::write(stream, scene->name_);
}

// -----------------------------------------------------------------------------------------------------------------------------
ScenePtr Scene::deserializeFriendly(std::istream& stream)
{
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto scene = frser::createAsset<Scene>(props);
  return scene;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Scene::serializeFriendly(Asset* asset, std::ostream& stream)
{
  namespace pt = boost::property_tree;
  auto scene = assetCast<Scene>(asset);
  pt::ptree props;
  frser::write(props, frser::NAME_PNAME, scene->name_);
  pt::write_json(stream, props);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Scene::resolveRefs(Asset*, Storage*)
{
}

} // !namespace assets
} // !namespace yaga
