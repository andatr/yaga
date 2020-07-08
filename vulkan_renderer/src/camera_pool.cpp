#include "precompiled.h"
#include "camera_pool.h"

namespace yaga {
namespace vk {

// -------------------------------------------------------------------------------------------------------------------------
CameraPool::CameraPool(Device* device, VmaAllocator allocator, Swapchain* swapchain, VkDescriptorPool descriptorPool) :
  device_(device), vkDevice_(**device), allocator_(allocator), descriptorPool_(descriptorPool),
  frames_(static_cast<uint32_t>(swapchain->frameBuffers().size())), mainCamera_(nullptr)
{
  createDescriptorLayout();
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::createDescriptorLayout()
{
  VkDescriptorSetLayoutBinding uboBinding{};
  uboBinding.binding = 0;
  uboBinding.descriptorCount = 1;
  uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboBinding.pImmutableSamplers = nullptr;
  uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  auto destroyLayout = [device = vkDevice_](auto layout) {
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    LOG(trace) << "Descriptor Set Layout destroyed";
  };

  VkDescriptorSetLayoutCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  info.bindingCount = 1;
  info.pBindings = &uboBinding;
  VkDescriptorSetLayout layout;
  VULKAN_GUARD(vkCreateDescriptorSetLayout(vkDevice_, &info, nullptr, &layout),
    "Could not create scene Descriptor Set Layout");
  layout_.set(layout, destroyLayout);
}

// ------------------------------------------------------------------------------------------------------------------------
CameraPool::~CameraPool()
{
  if (cameras_.empty()) {
    LOG(warning) << "Vulkan Camera Pool memory leak";
  }
}

// ------------------------------------------------------------------------------------------------------------------------
CameraPtr CameraPool::createCamera(Object* object, assets::Camera* asset)
{
  auto camera = std::make_unique<Camera>(object, asset, this, frames_);
  if (mainCamera_ == nullptr) {
    mainCamera_ = camera.get();
  }
  const auto cameraPtr = camera.get();
  cameras_.insert(cameraPtr);
  createDescriptorSets(cameraPtr);
  createBuffers(cameraPtr);
  updateDescriptorSets(cameraPtr);
  createCommandBuffers(cameraPtr);
  return camera;
}

// ------------------------------------------------------------------------------------------------------------------------
void CameraPool::removeCamera(Camera* camera)
{
  vkDeviceWaitIdle(vkDevice_);
  if (mainCamera_ == camera) {
    mainCamera_ = nullptr;
  }
  cameras_.erase(camera);
}

// ------------------------------------------------------------------------------------------------------------------------
void CameraPool::clear() // would be strange to make it const
{
  if (!cameras_.empty()) {
    THROW("Can not clear Camera Pool while its components are still in use");
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::createDescriptorSets(Camera* camera) const
{
  std::vector<VkDescriptorSetLayout> layouts(frames_, *layout_);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool_;
  allocInfo.descriptorSetCount = frames_;
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> sets(frames_);
  VULKAN_GUARD(vkAllocateDescriptorSets(vkDevice_, &allocInfo, sets.data()),
    "Could not allocate Uniform Descriptor Sets");
  for (size_t i = 0; i < frames_; ++i) {
    camera->frames_[i].descriptor = sets[i];
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::createBuffers(Camera* camera) const
{
  VkBufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  info.size = sizeof(UniformObject);
  info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  for (uint32_t i = 0; i < frames_; i++) {
    camera->frames_[i].buffer = std::make_unique<Buffer>(allocator_, info, allocInfo);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::updateDescriptorSets(Camera* camera) const
{
  std::vector<VkWriteDescriptorSet> writers(frames_);
  for (uint32_t i = 0; i < frames_; i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = **camera->frames_[i].buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformObject);

    writers[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writers[i].dstSet = camera->frames_[i].descriptor;
    writers[i].dstBinding = 0;
    writers[i].dstArrayElement = 0;
    writers[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writers[i].descriptorCount = 1;
    writers[i].pBufferInfo = &bufferInfo;
  }
  vkUpdateDescriptorSets(vkDevice_, frames_, writers.data(), 0, nullptr);
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::createCommandBuffers(Camera* camera) const
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = device_->commandPool();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = frames_;

  std::vector<VkCommandBuffer> commands(frames_);
  VULKAN_GUARD(vkAllocateCommandBuffers(vkDevice_, &allocInfo, commands.data()),
    "Could not allocate Camera Command Buffers");
  for (size_t i = 0; i < frames_; ++i) {
    camera->frames_[i].command = commands[i];
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::update(uint32_t frameIndex)
{
  for (auto& camera : cameras_) {
    auto& frame = camera->frame(frameIndex);
    if (frame.dirty) {
      updateBuffers(camera, frame);
      frame.dirty = false;
    }
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void CameraPool::updateBuffers(Camera* camera, Camera::Frame& frame)
{
  void* mappedData;
  vmaMapMemory(allocator_, frame.buffer->allocation(), &mappedData);
  memcpy(mappedData, &camera->uniform_, sizeof(UniformObject));
  vmaUnmapMemory(allocator_, frame.buffer->allocation());
}

} // !namespace vk
} // !namespace yaga