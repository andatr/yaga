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
void Material::ResolveRefs(Database* db)
{
  vertShader_ = db->Get<Shader>(vertName_);
  fragShader_ = db->Get<Shader>(fragName_);
}

// -------------------------------------------------------------------------------------------------------------------------
Material& Material::VertexShader(Shader* shader)
{
  vertShader_ = shader;
  vertName_ = vertShader_->Name();
  return *this;
}

// -------------------------------------------------------------------------------------------------------------------------
Material& Material::FragmentShader(Shader* shader)
{
  fragShader_ = shader;
  fragName_ = fragShader_->Name();
  return *this;
}

// -------------------------------------------------------------------------------------------------------------------------
size_t Material::Serialize(Asset* asset, std::ostream& stream, bool)
{
  auto material = dynamic_cast<Material*>(asset);
  if (!material)
    THROW("Material serializer was given the wrong asset");
  auto size = Serializer::Serialize(stream, material->vertName_);
  return size + Serializer::Serialize(stream, material->fragName_);
}

// -------------------------------------------------------------------------------------------------------------------------
MaterialPtr Material::Deserialize(const std::string& name, std::istream& stream, size_t, bool binary)
{
  auto material = std::make_unique<Material>(name);
  if (binary) {
    material->vertName_ = Serializer::DeserializeString(stream);
    material->fragName_ = Serializer::DeserializeString(stream);
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

