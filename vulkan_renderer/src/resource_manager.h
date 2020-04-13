#ifndef YAGA_VULKAN_RENDERER_SRC_RESOURCE_MANAGER
#define YAGA_VULKAN_RENDERER_SRC_RESOURCE_MANAGER

#include <map>
#include <memory>
#include <vector>

#include "buffer.h"
#include "device.h"
#include "image_pool.h"
#include "material_pool.h"
#include "mesh_pool.h"
#include "swapchain.h"
#include "vulkan.h"
#include "engine/scene.h"
#include "engine/asset/application.h"
#include "engine/asset/scene.h"
#include "engine/asset/shader.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class ResourceManager
{
public:
  explicit ResourceManager(Device* device, VmaAllocator allocator, Swapchain* swapchain, asset::Application* limits);
  void swapchain(Swapchain* swapchain);
  Scene* changeScene(asset::Scene* asset);
private:
  void createUniformBuffers();
  void updateUniformBuffers(Swapchain* swapchain);
private:
  Device* device_;
  VkDevice vkDevice_;
  VmaAllocator allocator_;
  uint32_t frames_;
  ImagePoolPtr imagePool_;
  MaterialPoolPtr materialPool_;
  MeshPoolPtr meshPool_;
  std::vector<BufferPtr> uniformBuffers_;
  std::vector<VkDescriptorSet> uniformDescriptorSet_;
  ScenePtr scene_;
  std::vector<ObjectPtr> objects_;
};

typedef std::unique_ptr<ResourceManager> ResourceManagerPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_RESOURCE_MANAGER
