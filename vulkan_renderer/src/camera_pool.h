#ifndef YAGA_VULKAN_RENDERER_SRC_CAMERA_POOL
#define YAGA_VULKAN_RENDERER_SRC_CAMERA_POOL

#include <memory>
#include <unordered_set>

#include "camera.h"
#include "device.h"
#include "swapchain.h"
#include "vulkan.h"
#include "utility/auto_destructor.h"

namespace yaga {
namespace vk {

class CameraPool
{
public:
  explicit CameraPool(Device* device, VmaAllocator allocator, Swapchain* swapchain, VkDescriptorPool descriptorPool);
  ~CameraPool();
  // void swapchain(Swapchain* swapchain);
  VkDescriptorSetLayout layout() const { return *layout_; }
  const std::unordered_set<Camera*>& cameras() const { return cameras_; }
  CameraPtr createCamera(Object* object, assets::Camera* asset);
  void removeCamera(Camera* camera);
  void clear();
  Camera* mainCamera() const { return mainCamera_; }
  void mainCamera(Camera* camera) { mainCamera_ = camera; }
  void update(uint32_t frameIndex);

private:
  void createDescriptorLayout();
  void createDescriptorSets(Camera* camera) const;
  void createBuffers(Camera* camera) const;
  void updateDescriptorSets(Camera* camera) const;
  void createCommandBuffers(Camera* camera) const;
  void updateBuffers(Camera* camera, Camera::Frame& frame);

private:
  Device* device_;
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  VkDescriptorPool descriptorPool_;
  uint32_t frames_;
  AutoDestructor<VkDescriptorSetLayout> layout_;
  std::unordered_set<Camera*> cameras_;
  Camera* mainCamera_;
};

typedef std::unique_ptr<CameraPool> CameraPoolPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_CAMERA_POOL
