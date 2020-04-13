#ifndef YAGA_ENGINE_ASSET_MATERIAL
#define YAGA_ENGINE_ASSET_MATERIAL

#include <memory>

#include "engine/asset/serializer.h"
#include "engine/asset/shader.h"
#include "engine/asset/texture.h"

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
  const std::vector<Texture*>& textures() const { return textures_; }
protected:
  void resolveRefs(Database*) override;
public:
  static const SerializationInfo serializationInfo;
  static MaterialPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static MaterialPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  Shader* vertShader_;
  Shader* fragShader_;
  std::vector<Texture*> textures_;
  std::string vertName_;
  std::string fragName_;
  std::vector<std::string> textureNames_;  
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_MATERIAL
