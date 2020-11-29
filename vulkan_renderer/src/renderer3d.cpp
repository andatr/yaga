#include "precompiled.h"
#include "renderer3d.h"
#include "rendering_context.h"
#include "engine/object.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3D::Renderer3D(Object* obj, RenderingContext* pool) :
  yaga::Renderer3D(obj), pool_(pool), material_(obj->getComponent<Material>()), mesh_(obj->getComponent<Mesh>()),
  transform_(obj->getComponent<Transform>())
{}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3D::~Renderer3D()
{
  pool_->removeRenderer3D(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Renderer3D::canRender() const
{
  return !!material_ && !!mesh_ && !!transform_ && object_->active();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer3D::onComponentAdd(Component* component)
{
  if (!material_) {
    material_ = dynamic_cast<Material*>(component);
  }
  if (!mesh_) {
    mesh_ = dynamic_cast<Mesh*>(component);
  }
  if (!transform_) {
    transform_ = dynamic_cast<Transform*>(component);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
PushConstantVertex Renderer3D::pushConstant() const
{
  PushConstantVertex result{};
  result.model = transform_ ? transform_->world() : glm::mat4(1.0f);
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer3D::onComponentRemove(Component* component)
{
  if (component == material_) {
    material_ = nullptr;
  }
  if (component == mesh_) {
    mesh_ = nullptr;
  }
  if (component == transform_) {
    transform_ = nullptr;
  }
}

} // !namespace vk
} // !namespace yaga