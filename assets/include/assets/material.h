#ifndef YAGA_ASSETS_MATERIAL
#define YAGA_ASSETS_MATERIAL

#include <istream>
#include <memory>
#include <vector>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "assets/shader.h"
#include "assets/texture.h"

namespace yaga {
namespace assets {

class Material;
typedef std::unique_ptr<Material> MaterialPtr;

enum class MaterialProperty
{
  vertexShader,
  fragmentShader,
  textures
};

class Material
  : public Asset
  , public UpdateNotifier<MaterialProperty>
{
public:
  typedef std::vector<Texture*> Textures;
  typedef std::function<void(Textures&)> TextureUpdater;

public:
  explicit Material(const std::string& name);
  virtual ~Material();
  Shader* vertexShader() const { return vertShader_; }
  Shader* fragmentShader() const { return fragShader_; }
  const Textures& textures() const { return textures_; }
  void vertexShader(Shader* shader);
  void fragmentShader(Shader* shader);
  void textures(const Textures& textures);
  void textures(TextureUpdater textures);

public:
  static const SerializationInfo serializationInfo;
  static MaterialPtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static MaterialPtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  Shader* vertShader_;
  Shader* fragShader_;
  Textures textures_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_MATERIAL
