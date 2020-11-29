#include "precompiled.h"
#include "assets/material.h"

namespace yaga {
namespace assets {

const SerializationInfo Material::serializationInfo = {
  (uint32_t)StandardAssetId::material,
  { "ymat" },
  &Material::deserializeBinary,
  &Material::deserializeFriendly
};

// -----------------------------------------------------------------------------------------------------------------------------
Material::Material(const std::string& name) : Asset(name), vertShader_(nullptr), fragShader_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::vertexShader(Shader* shader)
{
  vertShader_ = shader;
  fireUpdate(MaterialProperty::vertexShader);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::fragmentShader(Shader* shader)
{
  fragShader_ = shader;
  fireUpdate(MaterialProperty::fragmentShader);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::textures(const Textures& textures)
{
  textures_ = textures;
  fireUpdate(MaterialProperty::textures);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::textures(TextureUpdater handler)
{
  handler(textures_);
  fireUpdate(MaterialProperty::textures);
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver)
{
  namespace pt = boost::property_tree;
  auto material = std::make_unique<Material>(name);
  pt::ptree props;
  pt::read_json(path, props);
  material->vertShader_ = resolver.getAsset<Shader>(props.get<std::string>("vertexShader"));
  material->fragShader_ = resolver.getAsset<Shader>(props.get<std::string>("fragmentShader"));
  if (props.count("textures") > 0) {
    auto textures = props.get_child("textures");
    material->textures_.reserve(textures.size());
    for (const auto& texture : textures) {
      material->textures_.push_back(resolver.getAsset<Texture>(texture.second.get_value<std::string>()));
    }
  }
  return material;
}

} // !namespace assets
} // !namespace yaga
