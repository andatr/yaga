#ifndef YAGA_ASSETS_MATERIAL
#define YAGA_ASSETS_MATERIAL

#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "assets/shader.h"
#include "assets/texture.h"

namespace yaga {
namespace assets {

class Material;
typedef std::shared_ptr<Material> MaterialPtr;

class Material : public Asset
{
public:
  typedef std::vector<TexturePtr> Textures;
  typedef std::function<void(Textures&)> TextureUpdater;
  struct PropertyInfo
  {
    static const int vertexShader   = 0;
    static const int fragmentShader = 1;
    static const int textures       = 2;
  };

public:
  explicit Material(const std::string& name);
  virtual ~Material();
  ShaderPtr vertexShader()   const { return vertShader_; }
  ShaderPtr fragmentShader() const { return fragShader_; }
  const Textures& textures() const { return textures_;   }
  void vertexShader  (ShaderPtr shader);
  void fragmentShader(ShaderPtr shader);
  void textures(const Textures& textures);
  void textures(TextureUpdater textures);
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::material;
  static MaterialPtr deserializeBinary  (std::istream& stream);
  static MaterialPtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  ShaderPtr vertShader_;
  ShaderPtr fragShader_;
  Textures textures_;
  std::string vertShaderName_;
  std::string fragShaderName_;
  std::vector<std::string> textureNames_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_MATERIAL
