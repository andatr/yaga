#ifndef YAGA_RENDERER_SRC_MATERIAL
#define YAGA_RENDERER_SRC_MATERIAL

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "swapchain.h"
#include "utility/array.h"
#include "utility/auto_destructor.h"
#include "asset/material.h"

namespace yaga
{

class Material : private boost::noncopyable
{
public:
  Material(Device* device, Swapchain* swapchain, asset::Material* asset);
  VkRenderPass renderPass() const { return *renderPass_; }
  VkPipeline pipeline() const { return *pipeline_; }
  const std::vector<VkFramebuffer>& frameBuffers() const { return frameBufferRefs_; }
private:
  void createPipeline();
  void createRenderPass();
  void createFramebuffers();
private:
  Device* device_;
  VkDevice vkDevice_;
  Swapchain* swapchain_;
  asset::Material* asset_;
  VkExtent2D size_;
  AutoDestructor<VkRenderPass> renderPass_;
  AutoDestructor<VkPipeline> pipeline_;
  std::vector<AutoDestructor<VkFramebuffer>> frameBuffers_;
  std::vector<VkFramebuffer> frameBufferRefs_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MATERIAL
