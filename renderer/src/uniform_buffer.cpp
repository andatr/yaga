#include "precompiled.h"
#include "uniform_buffer.h"
#include "asset/vertex.h"

namespace yaga
{
constexpr auto bufferSize = sizeof(UniformObject);

// -------------------------------------------------------------------------------------------------------------------------
UniformBuffer::UniformBuffer(VkDevice device, Allocator* allocator, VideoBuffer* videoBuffer):
  videoBuffer_(videoBuffer)
{
  buffers_.resize(videoBuffer->Textures().size());
  for (size_t i = 0; i < videoBuffer->Textures().size(); ++i) {
    buffers_[i] = std::make_unique<DeviceBuffer>(device, allocator, bufferSize,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void UniformBuffer::Update(uint32_t index)
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  UniformObject uniform = {};
  uniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  uniform.projection = glm::perspective(glm::radians(45.0f), videoBuffer_->Size().width / (float)videoBuffer_->Size().height, 0.1f, 10.0f);
  uniform.projection[1][1] *= -1;
  buffers_[index]->Update(&uniform, bufferSize);
}

} // !namespace yaga