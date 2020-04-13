#include "precompiled.h"
#include "asset/material.h"
#include "asset/serializer.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Material::serializationInfo = {
  6,
  { "ymat" },
  "Material",
  &Material::deserialize,
  &Material::deserializeFriendly
};

// -------------------------------------------------------------------------------------------------------------------------
Material::Material(const std::string& name) :
  Asset(name), vertShader_(nullptr), fragShader_(nullptr)
{
}
  
// -------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void Material::resolveRefs(Database* db)
{
  vertShader_ = db->get<Shader>(vertName_);
  fragShader_ = db->get<Shader>(fragName_);
  textures_.reserve(textureNames_.size());
  for (const auto& t : textureNames_) {
    textures_.push_back(db->get<Texture>(t));
  }
}

// -------------------------------------------------------------------------------------------------------------------------
Material& Material::vertexShader(Shader* shader)
{
  vertShader_ = shader;
  vertName_ = vertShader_->name();
  return *this;
}

// -------------------------------------------------------------------------------------------------------------------------
Material& Material::fragmentShader(Shader* shader)
{
  fragShader_ = shader;
  fragName_ = fragShader_->name();
  return *this;
}

// -------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserialize(const std::string&, std::istream&, size_t)
{
  THROW_NOT_IMPLEMENTED;
}

// -------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserializeFriendly(const std::string&, const std::string& name, std::istream& stream, size_t)
{
  namespace pt = boost::property_tree;
  auto material = std::make_unique<Material>(name);
  pt::ptree props;
  pt::read_json(stream, props);
  material->vertName_ = props.get<std::string>("vertexShader");
  material->fragName_ = props.get<std::string>("fragmentShader");
  if (props.count("textures") > 0) {
    auto textures = props.get_child("textures");
    material->textureNames_.reserve(textures.size());
    for (const auto& texture : textures) {
      material->textureNames_.push_back(texture.second.get_value<std::string>());
    }
  }
  return material;
}

} // !namespace asset
} // !namespace yaga

