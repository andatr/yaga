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
size_t Shader::Serialize(Asset* asset, std::ostream& stream, bool)
{
  auto shader = dynamic_cast<Shader*>(asset);
  if (!shader) {
    THROW("Shader serializer was given wrong asset");
  }
  stream.write(shader->code_.Data(), shader->code_.Size());
  return shader->code_.Size();
}

// -------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::Deserialize(const std::string& name, std::istream& stream, size_t size, bool)
{
  auto shader = std::make_unique<Shader>(name);
  ByteArray code(size);
  stream.read(code.Data(), code.Size());
  shader->Code(code);
  return std::move(shader);
}

} // !namespace asset
} // !namespace yaga
