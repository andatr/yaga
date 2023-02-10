#include "precompiled.h"
#include "vulkan_renderer/renderer3d.h"
#include "vulkan_renderer/renderer_3d_pool.h"
#include "engine/object.h"

namespace yaga {
namespace vk {

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3D::Renderer3D(Renderer3DPool* pool) :
  pool_(pool),
  object_(nullptr),
  material_(nullptr),
  mesh_(nullptr),
  transform_(nullptr)
{}

// -----------------------------------------------------------------------------------------------------------------------------
Renderer3D::~Renderer3D()
{
  pool_->remove(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Renderer3D::onAttached(Object* object)
{
  object_    = object;
  material_  = object->getComponent<Material>();
  mesh_      = object->getComponent<Mesh>();
  transform_ = object->getComponent<Transform>();
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