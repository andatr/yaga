#include "precompiled.h"
#include "assets/shader.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"
#include "friendly_serializer_helper.h"

namespace yaga {
namespace assets {

BINARY_SERIALIZER_REG(Shader)

// -----------------------------------------------------------------------------------------------------------------------------
Shader::Shader(const std::string& name) : Asset(name) 
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Shader::~Shader()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto shader = std::make_unique<Shader>(name);
  uint64_t size = 0;
  binser::read(stream, size);
  shader->bytes_.resize(size);
  stream.read(shader->bytes_.data(), size);
  return shader;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Shader::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto shader = assetCast<Shader>(asset);
  uint64_t size = shader->bytes_.size();
  binser::write(stream, size);
  stream.write(shader->bytes_.data(), size);
}

// -----------------------------------------------------------------------------------------------------------------------------
ShaderPtr Shader::deserializeFriendly(std::istream& stream)
{
  return deserializeBinary(stream);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Shader::serializeFriendly(Asset* asset, std::ostream& stream)
{
  serializeBinary(asset, stream);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Shader::resolveRefs(Asset*, Storage*)
{
}

} // !namespace assets
} // !namespace yaga
