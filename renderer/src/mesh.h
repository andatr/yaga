#ifndef YAGA_RENDERER_SRC_MESH
#define YAGA_RENDERER_SRC_MESH

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "device_buffer.h"
#include "asset/mesh.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class Mesh : private boost::noncopyable
{
public:
  explicit Mesh(Device* device, Allocator* allocator, asset::Mesh* asset);
  VkBuffer VertexBuffer() const { return **vertexBuffer_; }
  VkBuffer IndexBuffer() const { return **indexBuffer_; }
  const std::vector<Vertex>& Vertices() const { return asset_->Vertices(); }
  const std::vector<uint32_t>& Indices() const { return asset_->Indices(); }
private:
  void CreateVertexBuffer(VkDevice device, Allocator* allocator);
  void CreateIndexBuffer(VkDevice device, Allocator* allocator);
  void CopyBuffer(VkBuffer destination, VkBuffer source, VkDeviceSize size) const;
private:
  Device* device_;
  asset::Mesh* asset_;
  DeviceBufferPtr vertexBuffer_;
  DeviceBufferPtr indexBuffer_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MESH
