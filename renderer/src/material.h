#ifndef YAGA_RENDERER_SRC_MATERIAL
#define YAGA_RENDERER_SRC_MATERIAL

#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "video_buffer.h"
#include "utility/array.h"
#include "utility/auto_destroyer.h"
#include "asset/material.h"

namespace yaga
{

class Material : private boost::noncopyable
{
public:
  Material(Device* device, VideoBuffer* videoBuffer, VkCommandPool commandPool, asset::Material* asset);
  VkRenderPass RenderPass() const { return *renderPass_; }
  VkPipeline Pipeline() const { return *pipeline_; }
  const VkExtent2D& Resolution() const { return resolution_; }
  const std::vector<VkFramebuffer>& FrameBuffers() const { return frameBufferRefs_; }
private:
  void CreatePipeline(Device* device, VideoBuffer* videoBuffer, asset::Material* asset);
  void CreateLayout(VkDevice device);
  void CreateRenderPass(VkDevice device, VkFormat imageFormat);    
  void CreateFramebuffers(VkDevice device, VideoBuffer* videoBuffer);
private:
  VkExtent2D resolution_;
  AutoDestroyer<VkRenderPass> renderPass_;
  AutoDestroyer<VkPipelineLayout> layout_;
  AutoDestroyer<VkPipeline> pipeline_;
  Array<AutoDestroyer<VkFramebuffer>> frameBuffers_;
  std::vector<VkFramebuffer> frameBufferRefs_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MATERIAL
