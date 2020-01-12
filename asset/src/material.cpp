#include "precompiled.h"
#include "material.h"
#include "serializer.h"

namespace yaga
{
namespace asset
{

const AssetId Material::assetId = { "mt", 3 };

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
size_t Material::serialize(Asset* asset, std::ostream& stream, bool)
{
  auto material = dynamic_cast<Material*>(asset);
  if (!material)
    THROW("Material serializer was given the wrong asset");
  auto size = Serializer::serialize(stream, material->vertName_);
  return size + Serializer::serialize(stream, material->fragName_);
}

// -------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::deserialize(const std::string& name, std::istream& stream, size_t, bool binary)
{
  auto material = std::make_unique<Material>(name);
  if (binary) {
    material->vertName_ = Serializer::deserializeString(stream);
    material->fragName_ = Serializer::deserializeString(stream);
  }
  else {
    std::getline(stream, material->vertName_);
    std::getline(stream, material->fragName_);
    boost::erase_all(material->vertName_, "\r");
    boost::erase_all(material->fragName_, "\r");
  }
  return material;
}

} // !namespace asset
} // !namespace yaga

