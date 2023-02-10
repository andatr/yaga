#include "precompiled.h"
#include "engine/object.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
Object::Object(std::string name) :
  name_(name),
  active_(true)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Object::~Object()
{
  for (const auto& component : components_) {
    for (const auto& removed : components_) {
      if (removed != component) {
        component.second->onComponentRemove(removed.second.get());
      }
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Object::removeComponent(Component* component)
{
  const auto& id = typeid(*component);
  auto it = components_.find(id);
  if (it == components_.end() || it->second.get() != component) {
    THROW("Object has no components of type \"%1%\"", id.name());
  }
  for (const auto& c : components_) {
    if (c.second != it->second) {
      c.second->onComponentRemove(component);
    }
  }
  components_.erase(it);
  makeProperties();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Object::makeProperties()
{
  properties_.clear();
  for (auto& component : components_) {
    const auto ptr = component.second.get();
    addProperty(ptr->name(), ptr);
  }
}

} // !namespace yaga
