#ifndef YAGA_VULKAN_RENDERER_CAMERA_POOL
#define YAGA_VULKAN_RENDERER_CAMERA_POOL

#include <memory>
#include <unordered_map>

#include "vulkan_renderer/camera.h"

namespace yaga {
namespace vk {

class CameraPool
{
public:
  explicit CameraPool();
  ~CameraPool();
  CameraPtr get(assets::CameraPtr camera);
  void clear();
  void remove(Camera* camera);
  Camera* mainCamera() const { return mainCamera_; };
  void mainCamera(Camera* value) { mainCamera_ = value; }

private:
  size_t counter_;
  Camera* mainCamera_;
};

typedef std::unique_ptr<CameraPool> CameraPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_CAMERA_POOL
