#ifndef YAGA_VULKAN_RENDERER_RENDERER_3D
#define YAGA_VULKAN_RENDERER_RENDERER_3D

#include <memory>

#include "vulkan_renderer/material.h"
#include "vulkan_renderer/mesh.h"
#include "vulkan_renderer/uniform.h"
#include "engine/renderer3d.h"

namespace yaga {
namespace vk {

class Renderer3DPool;

class Renderer3D : public yaga::Renderer3D
{
public:
  explicit Renderer3D(Renderer3DPool* pool);
  virtual ~Renderer3D();
  bool canRender() const;
  Mesh* mesh() const { return mesh_; }
  Material* material() const { return material_; }
  Transform* transform() const { return transform_; }
  PushConstantVertex pushConstant() const;

protected:
  void onAttached(Object* object) override;

private:
  void onComponentAdd(Component* component) override;
  void onComponentRemove(Component* component) override;

protected:
  Renderer3DPool* pool_;
  Object* object_;
  Material* material_;
  Mesh* mesh_;
  Transform* transform_;
};

typedef std::unique_ptr<Renderer3D> Renderer3DPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_RENDERER_3D
