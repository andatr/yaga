#include "precompiled.h"
#include "vulkan_renderer/camera.h"
#include "vulkan_renderer/camera_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Camera::Camera(CameraPool* pool, assets::CameraPtr asset) :
  yaga::Camera(asset),
  pool_(pool)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Camera::~Camera()
{
  pool_->remove(this);
}

} // !namespace vk
} // !namespace yaga
