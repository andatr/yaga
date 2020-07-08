#ifndef YAGA_ASSETS_SCENE
#define YAGA_ASSETS_SCENE

#include <istream>
#include <memory>

#include "assets/asset.h"
#include "assets/model.h"
#include "assets/serializer.h"

namespace yaga {
namespace assets {

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
  static ScenePtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static ScenePtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  Model* model_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_SCENE
