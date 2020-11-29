#include "precompiled.h"
#include "assets/shader.h"

namespace yaga {
namespace assets {

const SerializationInfo Shader::serializationInfo = {
  (uint32_t)StandardAssetId::shader,
  { "spv" },
  &Shader::deserializeBinary,
  &Shader::deserializeFriendly
};

// -----------------------------------------------------------------------------------------------------------------------------
Shader::Shader(const std::string& name) : Asset(name) 
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Shader::~Shader()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver&)
{
  auto shader = std::make_unique<Shader>(name);
  ByteArray code(size);
  stream.read(code.data(), code.size());
  shader->code(code);
  return shader;
}

// -----------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver)
{
  auto size = boost::filesystem::file_size(path);
  std::ifstream stream(path, std::ios::in | std::ios::binary);
  return deserializeBinary(name, stream, size, resolver);
}

} // !namespace assets
} // !namespace yaga
