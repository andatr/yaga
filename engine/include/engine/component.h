#ifndef YAGA_ENGINE_COMPONENT
#define YAGA_ENGINE_COMPONENT

#include <functional>
#include <memory>
#include "utility/update_notifier.h"

#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>

namespace yaga
{

class Component : private boost::noncopyable, public UpdateNotifier<uint32_t>
{
friend class Object;
public:
  explicit Component(Object* obj);
  virtual ~Component() {}
  Object* object() const { return object_; }
private:
  virtual void onComponentAdd(Component*) {}
  virtual void onComponenRemove(Component*) {}
protected:
  Object* object_;
};

typedef std::unique_ptr<Component> ComponentPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_COMPONENT
