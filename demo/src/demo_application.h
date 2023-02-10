#ifndef YAGA_DEMO_SRC_DEMO_APPLICATION
#define YAGA_DEMO_SRC_DEMO_APPLICATION

#include <map>
#include <memory>
#include <string>

#include "assets/asset.h"
#include "assets/mesh.h"
#include "assets/transform.h"
#include "engine/application.h"
#include "engine/camera.h"
#include "engine/object.h"

namespace yaga {
namespace demo {

class DemoApplication : public yaga::Application
{
public:
  explicit DemoApplication(assets::Serializer* serializer);
  virtual ~DemoApplication();
  void init(Context* context, Input* input) override;
  void resize()   override;
  bool loop()     override;
  void stop()     override;
  void shutdown() override;
  void gui()      override;

protected:
  template<typename T, typename... Args>
  T* addAsset(Args... args);
  template<typename T, typename... Args>
  T* addObject(Args... args);

private:
  void createCamera();

protected:
  bool running_;
  assets::Serializer* serializer_;
  assets::StoragePtr assets_;
  Context* context_;
  Input* input_;
  std::map<std::string, ObjectPtr> objects_;
  Camera* camera_;
  assets::Transform* cameraTransform_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T, typename... Args>
T* DemoApplication::addAsset(Args... args)
{
  auto asset = std::make_unique<T>(args...);
  auto ptr = asset.get();
  assets_->put(std::move(asset));
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T, typename... Args>
T* DemoApplication::addObject(Args... args)
{
  auto object = std::make_unique<T>(args...);
  auto ptr = object.get();
  objects_[object->name()] = std::move(object);
  return ptr;
}

} // !namespace demo
} // !namespace yaga

#endif // !YAGA_DEMO_SRC_DEMO_APPLICATION
