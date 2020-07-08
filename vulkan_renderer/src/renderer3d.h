#ifndef YAGA_VULKAN_RENDERER_RENDERER_3D
#define YAGA_VULKAN_RENDERER_RENDERER_3D

#include <memory>

#include "material.h"
#include "mesh.h"
#include "uniform.h"
#include "engine/renderer3d.h"

namespace yaga {
namespace vk {

class RenderingContext;

class Renderer3D : public yaga::Renderer3D
{
public:
  explicit Renderer3D(Object* obj, RenderingContext* pool);
  virtual ~Renderer3D();
  bool canRender() const;
  Mesh* mesh() const { return mesh_; }
  Material* material() const { return material_; }
  Transform* transform() const { return transform_; }
  PushConstantVertex pushConstant() const;

private:
  void onComponentAdd(Component* component) override;
  void onComponentRemove(Component* component) override;

protected:
  RenderingContext* pool_;
  Material* material_;
  Mesh* mesh_;
  Transform* transform_;
};

typedef std::unique_ptr<Renderer3D> Renderer3DPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDERER_3D
