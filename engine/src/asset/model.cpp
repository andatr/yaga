#include "precompiled.h"
#include "asset/model.h"
#include "asset/serializer.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Model::serializationInfo = {
  7,
  { "ymod" },
  "Model",
  &Model::deserialize,
  &Model::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Model::Model(const std::string& name) :
  Asset(name), material_(nullptr), mesh_(nullptr)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Model::~Model()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Model::resolveRefs(Database* db)
{
  material_ = db->get<Material>(materialName_);
  mesh_ = db->get<Mesh>(meshName_);
}

// -------------------------------------------------------------------------------------------------------------------------
ModelPtr Model::deserialize(const std::string& name, std::istream& stream, size_t size)
{
  return deserializeFriendly("", name, stream, size);
}

// -------------------------------------------------------------------------------------------------------------------------
ModelPtr Model::deserializeFriendly(const std::string&, const std::string& name, std::istream& stream, size_t)
{
  namespace pt = boost::property_tree;
  auto model = std::make_unique<Model>(name);
  pt::ptree props;
  pt::read_json(stream, props);
  model->materialName_ = props.get<std::string>("material");
  model->meshName_ = props.get<std::string>("mesh");
  return model;
}

} // !namespace asset
} // !namespace yaga
