#ifndef YAGA_VULKAN_PLAYER_SRC_DECODER_COLOR_SPACE_CONVERTER
#define YAGA_VULKAN_PLAYER_SRC_DECODER_COLOR_SPACE_CONVERTER

#include <memory>
#include <vector>

#include "frame.h"
#include "vulkan_renderer/image_view.h"
#include "vulkan_renderer/swapchain.h"
#include "vulkan_renderer/vulkan.h"

namespace yaga {
namespace vk {

class ColorSpaceConverter
{
public:
  struct Images
  {
    ImageView chroma;
    ImageView luma;
    ImageView dest;
  };

public:
  ColorSpaceConverter(Swapchain* swapchain, VmaAllocator allocator, size_t images, VkImageLayout destLayout);
  void convert(int index);
  void update(const std::vector<Images>& images, FormatPtr format);
  void waitIdle();

private:
  void createDescriptorLayout();
  void createPipelineLayout();
  void createShaders();
  void createPipeline();
  void createDescriptorPool();
  void createDescriptors();
  void updateDescriptors();
  void createCommandPool();
  void createCommand();

private:
  Swapchain* swapchain_;
  VmaAllocator allocator_;
  std::vector<Images> images_;
  VkImageLayout destLayout_;
  AutoDestructor<VkDescriptorSetLayout> descriptorLayout_;
  AutoDestructor<VkPipelineLayout> pipelineLayout_;
  AutoDestructor<VkShaderModule> shader_;
  AutoDestructor<VkPipeline> pipeline_;
  AutoDestructor<VkDescriptorPool> descriptorPool_;
  std::vector<VkDescriptorSet> descriptors_;
  AutoDestructor<VkCommandPool> commandPool_;
  AutoDestructor<VkCommandBuffer> command_;
  AutoDestructor<VkFence> fence_;
  FormatPtr format_;
};

typedef std::unique_ptr<ColorSpaceConverter> ColorSpaceConverterPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_PLAYER_SRC_DECODER_COLOR_SPACE_CONVERTER
