#ifndef YAGA_RENDERER_SRC_MESH
#define YAGA_RENDERER_SRC_MESH

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "asset/mesh.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class Mesh : private boost::noncopyable
{
public:
  explicit Mesh(Device* device, Allocator* allocator, asset::Mesh* asset);
  void Rebuild();
  VkBuffer VertexBuffer() const { return *vertexBuffer_; }
  VkBuffer IndexBuffer() const { return *indexBuffer_; }
  const std::vector<Vertex>& Vertices() const { return asset_->Vertices(); }
  const std::vector<uint16_t>& Indices() const { return asset_->Indices(); }
private:
  AutoDestroyer<VkBuffer> CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const;
  void CreateVertexBuffer();
  void CreateIndexBuffer();
  void CopyBuffer(VkBuffer destination, VkBuffer source, VkDeviceSize size) const;
private:
  Device* device_;
  Allocator* allocator_;
  asset::Mesh* asset_;
  AutoDestroyer<VkDeviceMemory> vertexMemory_;
  AutoDestroyer<VkBuffer> vertexBuffer_;
  AutoDestroyer<VkBuffer> indexBuffer_;
  AutoDestroyer<VkDeviceMemory> indexMemory_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MESH
