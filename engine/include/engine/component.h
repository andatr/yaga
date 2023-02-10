#ifndef YAGA_ENGINE_COMPONENT
#define YAGA_ENGINE_COMPONENT

#include "utility/compiler.h"

#include <memory>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "utility/prop_info.h"

namespace yaga {

class Component
  : private boost::noncopyable
  , public PropInfo
{
friend class Object;

public:
  virtual ~Component() {}
  virtual const std::string& name() = 0;

protected:
  virtual void onAttached(Object* object);

private:
  virtual void onComponentAdd(Component*) {}
  virtual void onComponentRemove(Component*) {}
};

typedef std::unique_ptr<Component> ComponentPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_COMPONENT
