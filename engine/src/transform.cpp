#include "precompiled.h"
#include "engine/transform.h"

namespace yaga {
namespace {

const std::string componentName = "Transform";
const int parentConnection = 0;

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Transform::Transform(assets::TransformPtr asset) :
  asset_(asset),
  world_(1.0f),
  local_(1.0f),
  parent_(nullptr)
{
  addProperty("Local",  &local_);
  addProperty("World",  &world_);
  addProperty("Parent", &parent_);
  auto handler = [this](void*) { updateLocal(); };
  connections_.push_back(nullptr); // parent's 'World' property
  connections_.push_back(asset->properties(assets::Transform::PropertyIndex::rotation)   ->onUpdate(handler));
  connections_.push_back(asset->properties(assets::Transform::PropertyIndex::scale)      ->onUpdate(handler));
  connections_.push_back(asset->properties(assets::Transform::PropertyIndex::translation)->onUpdate(handler));
  handler(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
Transform::~Transform()
{
  for (const auto& child : children_) {
    child->parent(nullptr);
  }
  if (parent_) {
    parent_->children_.erase(this);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Transform::updateLocal()
{
  local_  = glm::scale(glm::mat4(1.0f), asset_->scale());
  local_ *= glm::mat4_cast(asset_->rotation());
  local_  = glm::translate(local_, asset_->translation());
  properties_[PropertyIndex::local]->update(this);
  updateWorld();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Transform::parent(Transform* parent)
{
  if (parent == parent_) return;
  if (parent_ != nullptr) {
    parent_->children_.erase(this);
    connections_[parentConnection] = nullptr;
  }
  if (parent != nullptr) {
    parent->children_.insert(this);
    connections_[parentConnection] = parent->properties(PropertyIndex::world)->onUpdate([this](void*) { updateWorld(); });
  }
  parent_ = parent;
  properties_[PropertyIndex::parent]->update(this);
  updateWorld();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Transform::updateWorld()
{
  if (parent_) {
    world_ = local_ * parent_->world();
  } else {
    world_ = local_;
  }
  properties_[PropertyIndex::world]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::string& Transform::name()
{
  return componentName;
}

} // !namespace yaga
