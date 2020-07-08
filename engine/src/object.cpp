#include "precompiled.h"
#include "object.h"

namespace yaga {

// ------------------------------------------------------------------------------------------------------------------------
Object::Object() : active_(true)
{
}

// ------------------------------------------------------------------------------------------------------------------------
Object::~Object()
{
  for (const auto& component : components_) {
    for (const auto& removed : components_) {
      component.second->onComponentRemove(removed.second.get());
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------
void Object::addComponent(ComponentPtr component)
{
  const auto& id = typeid(*component);
  if (components_.find(id) != components_.end()) {
    THROW(std::string("Object already has component of type ") + id.name());
  }
  for (const auto& c : components_) {
    c.second->onComponentAdd(component.get());
  }
  components_[id] = std::move(component);
}

// ------------------------------------------------------------------------------------------------------------------------
void Object::removeComponent(Component* component)
{
  const auto& id = typeid(*component);
  auto it = components_.find(id);
  if (it == components_.end() || it->second.get() != component) {
    THROW(std::string("Object does not have component of type ") + id.name());
  }
  for (const auto& c : components_) {
    c.second->onComponentRemove(component);
  }
  components_.erase(it);
}

} // !namespace yaga
