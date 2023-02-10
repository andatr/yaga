#ifndef YAGA_VULKAN_RENDERER_CAMERA
#define YAGA_VULKAN_RENDERER_CAMERA

#include <memory>
#include <string>

#include "engine/camera.h"

namespace yaga {
namespace vk {

class CameraPool;

class Camera : public yaga::Camera
{
public:
  Camera(CameraPool* pool, assets::CameraPtr asset);
  ~Camera();

protected:
  CameraPool* pool_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_CAMERA
