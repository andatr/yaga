#ifndef YAGA_ASSETS_SHADER
#define YAGA_ASSETS_SHADER

#include <istream>
#include <memory>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "utility/array.h"

namespace yaga {
namespace assets {

class Shader;
typedef std::unique_ptr<Shader> ShaderPtr;

class Shader : public Asset
{
public:
  explicit Shader(const std::string& name);
  virtual ~Shader();
  const ByteArray& code() const { return code_; }
  void code(const ByteArray& code) { code_ = code; }

public:
  static const SerializationInfo serializationInfo;
  static ShaderPtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static ShaderPtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  ByteArray code_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_SHADER
