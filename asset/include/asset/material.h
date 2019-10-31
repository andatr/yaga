#ifndef YAGA_ASSET_MATERIAL
#define YAGA_ASSET_MATERIAL

#include <memory>

#include "shader.h"
#include "serializer.h"

namespace yaga
{
namespace asset
{

class Material;
typedef std::unique_ptr<Material> MaterialPtr;

class Material : public Asset
{
public:
  explicit Material(const std::string& name);
  virtual ~Material();
  Shader* VertexShader() const { return vertShader_; }
  Shader* FragmentShader() const { return fragShader_; }
  Material& VertexShader(Shader* shader);
  Material& FragmentShader(Shader* shader);
public: // Serialization
  static const AssetId assetId;
  static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
  static MaterialPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
protected:
  void ResolveRefs(Database*) override;
private:
  Shader* vertShader_;
  Shader* fragShader_;
  std::string vertName_;
  std::string fragName_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_MATERIAL
