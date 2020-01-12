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
  Shader* vertexShader() const { return vertShader_; }
  Shader* fragmentShader() const { return fragShader_; }
  Material& vertexShader(Shader* shader);
  Material& fragmentShader(Shader* shader);
public: // Serialization
  static const AssetId assetId;
  static size_t serialize(Asset* asset, std::ostream& stream, bool binary);
  static MaterialPtr deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
protected:
  void resolveRefs(Database*) override;
private:
  Shader* vertShader_;
  Shader* fragShader_;
  std::string vertName_;
  std::string fragName_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_MATERIAL
