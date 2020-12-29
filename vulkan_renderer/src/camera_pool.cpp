#include "precompiled.h"
/*
#include "camera_pool.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
CameraPool::CameraPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPool::~CameraPool()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void CameraPool::mainCamera(yaga::Camera* camera)
{
  mainCamera_ = dynamic_cast<Camera*>(camera);
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraPtr CameraPool::get(Object* object)
{
  auto it = cameras_.find(asset);
  if (it != cameras_.end()) return it->second.get();

  auto camera = std::make_unique<Camera>(object, asset);
  
  auto ptr = camera.get();
  cameras_[asset] = std::move(camera);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
void CameraPool::clear()
{
  cameras_.clear(); 
}

} // !namespace vk
} // !namespace yaga
*/