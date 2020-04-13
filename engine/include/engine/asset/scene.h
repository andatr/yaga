#ifndef YAGA_ENGINE_ASSET_SCENE
#define YAGA_ENGINE_ASSET_SCENE

#include <memory>
#include <istream>
#include <ostream>

#include "engine/asset/asset.h"
#include "engine/asset/model.h"
#include "engine/asset/serializer.h"

namespace yaga
{
namespace asset
{

class Scene;
typedef std::unique_ptr<Scene> ScenePtr;

class Scene : public Asset
{
public:
  explicit Scene(const std::string& name);
  virtual ~Scene();
  Model* model() const { return model_; }
public:
  static const SerializationInfo serializationInfo;
  static ScenePtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static ScenePtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
protected:
  void resolveRefs(Database*) override;
private:
  Model* model_;
  std::string modelName_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_SCENE
