#ifndef YAGA_ENGINE_ASSET_SHADER
#define YAGA_ENGINE_ASSET_SHADER

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"
#include "utility/array.h"

namespace yaga
{
namespace asset
{

class Shader;
typedef std::unique_ptr<Shader> ShaderPtr;

class Shader : public Asset
{
public:
  explicit Shader(const std::string& name);
  virtual ~Shader();
  const ByteArray& code() const { return code_; }
  Shader& code(const ByteArray& code) { code_ = code; return *this; }
public:
  static const SerializationInfo serializationInfo;
  static ShaderPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static ShaderPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  ByteArray code_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_SHADER
