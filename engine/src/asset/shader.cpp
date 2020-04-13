#include "precompiled.h"
#include "asset/shader.h"

namespace yaga
{
namespace asset
{

const SerializationInfo Shader::serializationInfo = {
  2,
  { "spv" },
  "Vulkan compiled shader",
  &Shader::deserialize,
  &Shader::deserializeFriendly
};

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
ShaderPtr Shader::deserialize(const std::string& name, std::istream& stream, size_t size)
{
  return deserializeFriendly("", name, stream, size);
}

// -------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::deserializeFriendly(const std::string&, const std::string& name, std::istream& stream, size_t size)
{
  auto shader = std::make_unique<Shader>(name);
  ByteArray code(size);
  stream.read(code.data(), code.size());
  shader->code(code);
  return shader;
}

} // !namespace asset
} // !namespace yaga
