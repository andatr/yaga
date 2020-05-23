#include "precompiled.h"
#include "transform.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
Transform::Transform(Object* obj) :
  Component(obj), world_(1.0f), local_(1.0f), parent_(nullptr)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Transform::~Transform()
{
  for(const auto& child : children_) {
    child->parent(nullptr);
  }
  if (parent_) {
    parent_->children_.erase(this);
    parent_->onUpdate(parentConnection_);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
void Transform::local(const glm::mat4& m)
{
  local_ = m;
  updateWorld();
}

// ------------------------------------------------------------------------------------------------------------------------
void Transform::parent(Transform* parent)
{
  if (parent == parent_) return;
  if (parent_ != nullptr) {
    parent_->children_.erase(this);
    parent_->onUpdate(parentConnection_);
  }
  if (parent != nullptr) {
    parent->children_.insert(this);
    parentConnection_ = parent->onUpdate(std::bind(&Transform::updateWorld, this));
  }
  parent_ = parent;
  updateWorld();
}

// -------------------------------------------------------------------------------------------------------------------------
void Transform::updateWorld()
{
  if (parent_) {
    world_ = local_ * parent_->world();
  }
  else {
    world_ = local_;
  }
  fireUpdate(worldProperty);
}

} // !namespace yaga

