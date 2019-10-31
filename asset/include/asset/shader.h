#ifndef YAGA_ASSET_SHADER
#define YAGA_ASSET_SHADER

#include <memory>
#include <istream>
#include <ostream>

#include "asset.h"
#include "serializer.h"
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
  const ByteArray& Code() const { return code_; }
  Shader& Code(const ByteArray& code) { code_ = code; return *this; }
public: // Serialization
  static const AssetId assetId;
  static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
  static ShaderPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
private:
  ByteArray code_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_SHADER
