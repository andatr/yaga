#ifndef YAGA_ENGINE_OBJECT
#define YAGA_ENGINE_OBJECT

#include "utility/compiler.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "engine/component.h"
#include "utility/exception.h"
#include "utility/prop_info.h"

namespace yaga {

class Object
  : private boost::noncopyable
  , public PropInfo
{
public:
  typedef std::unordered_map<std::type_index, ComponentPtr> ComponentMap;

public:
  explicit Object(std::string name = "");
  virtual ~Object();
  const std::string& name() const { return name_; }
  bool active() const { return active_; }
  void active(bool a) { active_ = a; }
  template <typename C>
  C* addComponent(std::unique_ptr<C> component);
  void removeComponent(Component* component);
  const ComponentMap& components() const { return components_; }
  template <typename C>
  C* getComponent() const;

private:
  void makeProperties();

protected:
  std::string name_;
  bool active_;
  ComponentMap components_;
};

typedef std::unique_ptr<Object> ObjectPtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename C>
C* Object::getComponent() const
{
  auto it = components_.find(typeid(C));
  if (it != components_.end()) {
    return dynamic_cast<C*>(it->second.get());
  }
  return nullptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename C>
C* Object::addComponent(std::unique_ptr<C> component)
{
  const auto& id = typeid(*component);
  if (components_.find(id) != components_.end()) {
    THROW("Object already has a component of type \"%1%\"", id.name());
  }
  auto ptr = component.get();
  components_[id] = std::move(component);
  static_cast<Component*>(ptr)->onAttached(this);
  makeProperties();
  for (const auto& c : components_) {
    c.second->onComponentAdd(ptr);
  }
  return ptr;
}

} // !namespace yaga

#endif // !YAGA_ENGINE_OBJECT
