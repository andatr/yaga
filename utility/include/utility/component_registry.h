#ifndef YAGA_UTILITY_COMPONENT_REGISTRY
#define YAGA_UTILITY_COMPONENT_REGISTRY

#include "utility/compiler.h"

#include <functional>
#include <map>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class PropertyEditorFactory : private boost::noncopyable
{
public:
  typedef std::function<PropertyEditorPtr(IProperty*)> Factory;

public:
  static PropertyEditorFactory& instance();
  PropertyEditorPtr createEditor(IProperty* property);
  void registerFactory(std::type_index type, Factory factory);

private:
  std::map<std::type_index, Factory> factories_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Key, typename Value>
class RegistryEntry
{
public:
  static RegistryEntry<Key, Value>& instance();

private:
  RegistryEntry();
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Key, typename Value>
struct PropertyEditorRegistry
{
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Key, typename Value>
RegistryEntry<Key, Value>& RegistryEntry<Key, Value>::instance()
{
  static RegistryEntry<Key, Value> inst;
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Key, typename Value>
RegistryEntry<Key, Value>::RegistryEntry()
{
  PropertyEditorFactory::instance().registerFactory(typeid(Key), [](IProperty* prop) {
    return std::make_unique<Editor>(prop); 
  });
}

} // !namespace yaga

#endif // !YAGA_UTILITY_COMPONENT_REGISTRY
