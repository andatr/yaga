#ifndef YAGA_VULKAN_RENDERER_SRC_CAMERA_POOL
#define YAGA_VULKAN_RENDERER_SRC_CAMERA_POOL
/*
#include <memory>
#include <unordered_map>

#include "camera.h"
#include "vulkan.h"
#include "assets/camera.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class CameraPool
{
public:
  explicit CameraPool();
  ~CameraPool();
  CameraPtr get(Object* object, assets::Camera* asset);
  Camera* mainCamera() const { return mainCamera_; }
  void mainCamera(yaga::Camera* camera);

private:
  Camera* mainCamera_;
};

typedef std::unique_ptr<CameraPool> CameraPoolPtr;

} // !namespace vk
} // !namespace yaga
*/
#endif // !YAGA_VULKAN_RENDERER_SRC_CAMERA_POOL
