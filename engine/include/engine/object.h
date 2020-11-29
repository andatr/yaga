#ifndef YAGA_ENGINE_OBJECT
#define YAGA_ENGINE_OBJECT

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <boost/noncopyable.hpp>

#include "engine/component.h"

namespace yaga {

class Object : private boost::noncopyable
{
public:
  typedef std::unordered_map<std::type_index, ComponentPtr> ComponentMap;

public:
  explicit Object();
  virtual ~Object();
  bool active() const { return active_; }
  void active(bool a) { active_ = a; }
  void addComponent(ComponentPtr component);
  void removeComponent(Component* component);
  const ComponentMap& components() const { return components_; }
  template <typename T>
  T* getComponent() const;

protected:
  bool active_;
  ComponentMap components_;
};

typedef std::unique_ptr<Object> ObjectPtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* Object::getComponent() const
{
  auto it = components_.find(typeid(T));
  if (it != components_.end()) {
    return dynamic_cast<T*>(it->second.get());
  }
  return nullptr;
}

} // !namespace yaga

#endif // !YAGA_ENGINE_OBJECT
