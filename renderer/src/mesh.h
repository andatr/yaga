#ifndef YAGA_RENDERER_SRC_MESH
#define YAGA_RENDERER_SRC_MESH

#include <utility>
#include <vector>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "vertex.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class Mesh : private boost::noncopyable
{
public:
  explicit Mesh(Device* deivce);
  void Rebuild();
  VkBuffer Buffer() const { return *stageBuffer_; }
  const std::vector<Vertex>& Vertices() const { return vertices_; }
private:
  AutoDestroyer<VkBuffer> CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const;
  AutoDestroyer<VkDeviceMemory> AllocateMemory(VkBuffer buffer, VkDeviceSize size, VkMemoryPropertyFlags properties) const;
  void CreateVertexBuffer();
  void CreateStageBuffer();
private:
  std::vector<Vertex> vertices_;
  Device* device_;
  AutoDestroyer<VkDeviceMemory> vertexMemory_;
  AutoDestroyer<VkBuffer> vertexBuffer_;
  AutoDestroyer<VkDeviceMemory> stageMemory_;
  AutoDestroyer<VkBuffer> stageBuffer_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MESH
