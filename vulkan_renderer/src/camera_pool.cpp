#include "precompiled.h"
#include "vulkan_renderer/camera_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
CameraPool::CameraPool() :
  counter_(0),
  mainCamera_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPool::~CameraPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void CameraPool::remove(Camera* camera)
{
  if (mainCamera_ == camera) {
    mainCamera_ = nullptr;
  }
  if (counter_ > 0) {
    --counter_;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void CameraPool::clear() 
{
  if (counter_ != 0) {
    THROW("Not all cameras were returned to the pool");
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPtr CameraPool::get(assets::CameraPtr asset)
{
  auto camera = std::make_unique<Camera>(this, asset);
  if (!mainCamera_) {
    mainCamera_ = camera.get();
  }
  ++counter_;
  return camera;
}

} // !namespace vk
} // !namespace yaga