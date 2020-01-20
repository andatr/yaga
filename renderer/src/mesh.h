#ifndef YAGA_RENDERER_SRC_MESH
#define YAGA_RENDERER_SRC_MESH

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "device.h"
#include "device_buffer.h"
#include "engine/asset/mesh.h"
#include "utility/auto_destructor.h"

namespace yaga
{

class Mesh : private boost::noncopyable
{
public:
  explicit Mesh(Device* device, Allocator* allocator, asset::Mesh* asset);
  VkBuffer vertexBuffer() const { return **vertexBuffer_; }
  VkBuffer indexBuffer() const { return **indexBuffer_; }
  const std::vector<Vertex>& vertices() const { return asset_->vertices(); }
  const std::vector<uint32_t>& indices() const { return asset_->indices(); }
private:
  void createVertexBuffer();
  void createIndexBuffer();
  void copyBuffer(VkBuffer destination, VkBuffer source, VkDeviceSize size) const;
private:
  Device* device_;
  VkDevice vkDevice_;
  Allocator* allocator_;
  asset::Mesh* asset_;
  DeviceBufferPtr vertexBuffer_;
  DeviceBufferPtr indexBuffer_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_MESH
