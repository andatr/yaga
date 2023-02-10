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
typedef std::shared_ptr<Shader> ShaderPtr;

class Shader : public Asset
{
public:
  explicit Shader(const std::string& name);
  virtual ~Shader();
  const ByteArray& bytes() const { return bytes_; }
  void bytes(const ByteArray& value) { bytes_ = value; }
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::shader;
  static ShaderPtr deserializeBinary  (std::istream& stream);
  static ShaderPtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  ByteArray bytes_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_SHADER
