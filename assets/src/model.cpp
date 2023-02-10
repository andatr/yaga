#include "precompiled.h"
#include "assets/model.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {
namespace {

constexpr const char* MATERIAL_PNAME = "material";
constexpr const char* MESH_PNAME     = "mesh";

} // !namespace

BINARY_SERIALIZER_REG(Model)

// -----------------------------------------------------------------------------------------------------------------------------
Model::Model(const std::string& name) :
  Asset(name),
  material_(nullptr),
  mesh_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Model::~Model()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelPtr Model::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto model = std::make_unique<Model>(name);
  binser::read(stream, model->materialName_);
  binser::read(stream, model->meshName_);
  return model;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Model::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto model = assetCast<Model>(asset);
  binser::write(stream, model->materialName_);
  binser::write(stream, model->meshName_); 
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelPtr Model::deserializeFriendly(std::istream& stream)
{
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto model = frser::createAsset<Model>(props);
  frser::read(props, MATERIAL_PNAME, model->materialName_);
  frser::read(props, MESH_PNAME,     model->meshName_    );
  return model;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Model::serializeFriendly(Asset* asset, std::ostream& stream)
{
  namespace pt = boost::property_tree;
  auto model = assetCast<Model>(asset);
  pt::ptree props;
  frser::write(props, frser::NAME_PNAME, model->name_        );
  frser::write(props, MATERIAL_PNAME,    model->materialName_);
  frser::write(props, MESH_PNAME,        model->meshName_    );
  pt::write_json(stream, props);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Model::resolveRefs(Asset* asset, Storage* storage)
{
  auto model = assetCast<Model>(asset);
  if (!model->materialName_.empty()) {
    model->material_ = storage->get<Material>(model->materialName_);
  }
  if (!model->meshName_.empty()) {
    model->mesh_ = storage->get<Mesh>(model->meshName_);
  }
}

} // !namespace assets
} // !namespace yaga
