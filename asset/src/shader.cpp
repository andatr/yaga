#include "precompiled.h"
#include "shader.h"

namespace yaga
{
namespace asset
{

const AssetId Shader::assetId = { "spv", 2 };

// -------------------------------------------------------------------------------------------------------------------------
Shader::Shader(const std::string& name) :
  Asset(name)
{
}
  
// -------------------------------------------------------------------------------------------------------------------------
Shader::~Shader()
{
}

// -------------------------------------------------------------------------------------------------------------------------
size_t Shader::serialize(Asset* asset, std::ostream& stream, bool)
{
  auto shader = dynamic_cast<Shader*>(asset);
  if (!shader) {
    THROW("Shader serializer was given the wrong asset");
  }
  stream.write(shader->code_.data(), shader->code_.size());
  return shader->code_.size();
}

// -------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::deserialize(const std::string& name, std::istream& stream, size_t size, bool)
{
  auto shader = std::make_unique<Shader>(name);
  ByteArray code(size);
  stream.read(code.data(), code.size());
  shader->code(code);
  return std::move(shader);
}

} // !namespace asset
} // !namespace yaga
