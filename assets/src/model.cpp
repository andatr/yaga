#include "precompiled.h"
#include "assets/model.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

const SerializationInfo Model::serializationInfo = {
  (uint32_t)StandardAssetId::model,
  { "ymod" },
  &Model::deserializeBinary,
  &Model::deserializeFriendly
};

// -----------------------------------------------------------------------------------------------------------------------------
Model::Model(const std::string& name) : Asset(name), material_(nullptr), mesh_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Model::~Model()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelPtr Model::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelPtr Model::deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver)
{
  namespace pt = boost::property_tree;
  auto model = std::make_unique<Model>(name);
  pt::ptree props;
  pt::read_json(path, props);
  model->mesh_ = resolver.getAsset<Mesh>(props.get<std::string>("mesh"));
  model->material_ = resolver.getAsset<Material>(props.get<std::string>("material"));
  return model;
}

} // !namespace assets
} // !namespace yaga
