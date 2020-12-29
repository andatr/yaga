#include "precompiled.h"
#include "render_stage_3d.h"
#include "uniform.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
RenderStage3D::RenderStage3D(Swapchain* swapchain, VmaAllocator allocator, RenderPassPtr renderPass) :
  swapchain_(swapchain),
  allocator_(allocator),
  renderPass_(std::move(renderPass))
{
  createUniform();
}

// -----------------------------------------------------------------------------------------------------------------------------
RenderStage3D::~RenderStage3D()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
VkSemaphore RenderStage3D::render(Context* context, uint32_t frame, VkSemaphore waitFor)
{
  updateUniform(frame, context);
  auto command = renderPass_->beginRender(frame);
  for (const auto& object : context->renderers()) {
    if (object->canRender()) {
      renderObject(object, command, frame);
    }
  }
  return renderPass_->finishRender(frame, { renderPass_->finalStage(), waitFor }).semaphore;
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderStage3D::updateUniform(uint32_t frame, Context* context)
{
  if (!context->mainCamera()) return;
  const auto buffer = uniform_[frame].buffer.get();
  void* mappedData;
  vmaMapMemory(allocator_, buffer->allocation(), &mappedData); 
  auto uniform = (UniformObject*)mappedData;
  uniform->view       = context->mainCamera()->view();
  uniform->projection = context->mainCamera()->projection();
  uniform->screen = { 
    static_cast<float>(swapchain_->resolution().width),
    static_cast<float>(swapchain_->resolution().height) 
  };
  uniform->aspect = uniform->screen.x / uniform->screen.y;
  memcpy(mappedData, mappedData, sizeof(UniformObject));
  vmaUnmapMemory(allocator_, buffer->allocation());
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderStage3D::renderObject(Renderer3D* object, VkCommandBuffer command, uint32_t frame)
{
  const auto mesh     = object->mesh();
  const auto material = object->material();
  VkBuffer vertexBuffers[] = { **mesh->vertexBuffer() };
  VkDeviceSize offsets[] = { 0 };
  const auto& pconst = object->pushConstant();
  const auto& pipeline = material->wireframe() ? *material->pipeline().wireframe : *material->pipeline().main;
  const auto& layout = renderPass_->pipelineLayout();
  std::array<VkDescriptorSet, 2> descriptors{
    uniform_[frame].descriptor,
    material->pipeline().descriptors[frame]
  };
  vkCmdBindPipeline      (command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 2, descriptors.data(), 0, nullptr);
  vkCmdPushConstants     (command, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantVertex), &pconst);
  vkCmdBindVertexBuffers (command, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer   (command, **mesh->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed       (command, mesh->indexCount(), 1, 0, 0, 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
void RenderStage3D::createUniform()
{
  const auto frames = swapchain_->imageCount();
  uniform_.resize(frames);

  std::vector<VkDescriptorSetLayout> layouts(frames, renderPass_->uniformLayout());
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = swapchain_->device()->descriptorPool();
  allocInfo.descriptorSetCount = frames;
  allocInfo.pSetLayouts = layouts.data();

  std::vector<VkDescriptorSet> sets(frames);
  VULKAN_GUARD(vkAllocateDescriptorSets(**swapchain_->device(), &allocInfo, sets.data()),
    "Could not allocate Uniform Descriptor Sets");
  
  VkBufferCreateInfo binfo{};
  binfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  binfo.size = sizeof(UniformObject);
  binfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  binfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo ballocInfo{};
  ballocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  VkDescriptorBufferInfo bufferInfo = {};
  bufferInfo.offset = 0;
  bufferInfo.range = sizeof(UniformObject);

  VkWriteDescriptorSet writer{};
  writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writer.dstBinding = 0;
  writer.dstArrayElement = 0;
  writer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writer.descriptorCount = 1;
  writer.pBufferInfo = &bufferInfo;

  for (size_t i = 0; i < frames; ++i) {
    uniform_[i].descriptor = sets[i];
    uniform_[i].buffer = std::make_unique<Buffer>(allocator_, binfo, ballocInfo);
    bufferInfo.buffer = **uniform_[i].buffer;
    writer.dstSet = uniform_[i].descriptor;
    vkUpdateDescriptorSets(**swapchain_->device(), 1, &writer, 0, nullptr);
  }
}

} // !namespace vk
} // !namespace yaga