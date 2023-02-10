#include "precompiled.h"
#include "assets/material.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {
namespace {

constexpr const char* VERTEX_PNAME   = "vertexShader";
constexpr const char* FRAGMENT_PNAME = "fragmentShader";
constexpr const char* TEXTURE_PNAME  = "textures";

} // !namespace

BINARY_SERIALIZER_REG(Material)

// -----------------------------------------------------------------------------------------------------------------------------
Material::Material(const std::string& name) :
  Asset(name),
  vertShader_(nullptr),
  fragShader_(nullptr)
{
  addProperty("VertextShader",  vertShader_);
  addProperty("FragmentShader", fragShader_);
  addProperty("Textures",       &textures_);
}

// -----------------------------------------------------------------------------------------------------------------------------
Material::~Material()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::vertexShader(ShaderPtr shader)
{
  vertShader_ = shader;
  vertShaderName_ = shader ? shader->name() : "";
  properties_[PropertyInfo::vertexShader]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::fragmentShader(ShaderPtr shader)
{
  fragShader_ = shader;
  fragShaderName_ = shader ? shader->name() : "";
  properties_[PropertyInfo::fragmentShader]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::textures(const Textures& textures)
{
  textures_ = textures;
  textureNames_.resize(textures.size());
  for (size_t i = 0; i < textures.size(); ++i) {
    textureNames_[i] = textures[i]->name();
  }
  properties_[PropertyInfo::textures]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::textures(TextureUpdater handler)
{
  handler(textures_);
  textureNames_.resize(textures_.size());
  for (size_t i = 0; i < textures_.size(); ++i) {
    textureNames_[i] = textures_[i]->name();
  }
  properties_[PropertyInfo::textures]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto material = std::make_unique<Material>(name);
  binser::read(stream, material->vertShaderName_);
  binser::read(stream, material->fragShaderName_);
  binser::read(stream, material->textureNames_  );
  return material;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto material = assetCast<Material>(asset);
  binser::write(stream, material->name_);
  binser::write(stream, material->vertShaderName_);
  binser::write(stream, material->fragShaderName_); 
  binser::write(stream, material->textureNames_  );
}

// -----------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserializeFriendly(std::istream& stream)
{
  namespace pt = boost::property_tree;
  pt::ptree props;
  pt::read_json(stream, props);
  auto material = frser::createAsset<Material>(props);
  frser::read(props, VERTEX_PNAME,   material->vertShaderName_);
  frser::read(props, FRAGMENT_PNAME, material->fragShaderName_);
  frser::read(props, TEXTURE_PNAME,  material->textureNames_  );
  return material;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::serializeFriendly(Asset* asset, std::ostream& stream)
{
  namespace pt = boost::property_tree;
  auto material = assetCast<Material>(asset);
  pt::ptree props;
  frser::write(props, frser::NAME_PNAME, material->name_);
  frser::write(props, VERTEX_PNAME,      material->vertShaderName_);
  frser::write(props, FRAGMENT_PNAME,    material->fragShaderName_);
  frser::write(props, TEXTURE_PNAME,     material->textureNames_  );
  pt::write_json(stream, props);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Material::resolveRefs(Asset* asset, Storage* storage)
{
  auto material = assetCast<Material>(asset);
  if (!material->vertShaderName_.empty()) {
    material->vertShader_ = storage->get<Shader>(material->vertShaderName_);
  }
  if (!material->fragShaderName_.empty()) {
    material->fragShader_ = storage->get<Shader>(material->fragShaderName_);
  }
  if (!material->textureNames_.empty()) {
    material->textures_.resize(material->textureNames_.size());
    for (size_t i = 0; i < material->textureNames_.size(); ++i) {
      material->textures_[i] = storage->get<Texture>(material->textureNames_[i]);
    }
  }
}

} // !namespace assets
} // !namespace yaga
