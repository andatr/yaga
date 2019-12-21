#ifndef YAGA_RENDERER_SRC_MATERIAL
#define YAGA_RENDERER_SRC_MATERIAL

#include <memory>
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
  Material(Device* device, VideoBuffer* videoBuffer, asset::Material* asset);
  VkRenderPass RenderPass() const { return *renderPass_; }
  VkPipeline Pipeline() const { return *pipeline_; }
  const std::vector<VkFramebuffer>& FrameBuffers() const { return frameBufferRefs_; }
private:
  void CreatePipeline(Device* device, VideoBuffer* videoBuffer, asset::Material* asset);
  void CreateRenderPass(VkDevice device, VkFormat imageFormat, VkFormat depthFormat);
  void CreateFramebuffers(VkDevice device, VideoBuffer* videoBuffer);
private:
  VkExtent2D size_;
  AutoDestroyer<VkRenderPass> renderPass_;
  AutoDestroyer<VkPipeline> pipeline_;
  std::vector<AutoDestroyer<VkFramebuffer>> frameBuffers_;
  std::vector<VkFramebuffer> frameBufferRefs_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MATERIAL
