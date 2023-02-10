#ifndef YAGA_GUI_PROPERTY_EDITOR_REGISTRY
#define YAGA_GUI_PROPERTY_EDITOR_REGISTRY

#include "utility/compiler.h"

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <typeinfo>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "gui/property_editor.h"
#include "utility/property.h"

#define PROP_EDITOR_REG(E)                                                 \
  template<>                                                               \
  struct PropertyEditorRegistration<typename E::Type, E>                   \
  {                                                                        \
    static const PropertyEditorRegistryEntry<typename E::Type, E>& entry;  \
  };                                                                       \
  const PropertyEditorRegistryEntry<typename E::Type, E>&                  \
    PropertyEditorRegistration<typename E::Type, E>::entry =               \
      PropertyEditorRegistryEntry<typename E::Type, E>::instance();        \

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class PropertyEditorRegistry : private boost::noncopyable
{
public:
  typedef std::function<PropertyEditorPtr(IProperty*)> Factory;

public:
  static PropertyEditorRegistry& instance();
  PropertyEditorPtr createEditor(IProperty* property);
  void registerFactory(std::type_index type, Factory factory);
  void hideProperty(std::type_index type, const std::string& name);
  bool hidden(std::type_index type, const std::string& name);

private:
  PropertyEditorRegistry() {}

private:
  std::map<std::type_index, Factory> factories_;
  std::map<std::type_index, std::set<std::string>> hidden_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Prop, typename Editor>
class PropertyEditorRegistryEntry : private boost::noncopyable
{
public:
  static PropertyEditorRegistryEntry<Prop, Editor>& instance();

private:
  PropertyEditorRegistryEntry();
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Prop, typename Editor>
struct PropertyEditorRegistration
{
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Prop, typename Editor>
PropertyEditorRegistryEntry<Prop, Editor>& PropertyEditorRegistryEntry<Prop, Editor>::instance()
{
  static PropertyEditorRegistryEntry<Prop, Editor> inst;
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Prop, typename Editor>
PropertyEditorRegistryEntry<Prop, Editor>::PropertyEditorRegistryEntry()
{
  PropertyEditorRegistry::instance().registerFactory(typeid(Prop), [](IProperty* prop) {
    return std::make_unique<Editor>(prop); 
  });
}

} // !namespace yaga

#endif // !YAGA_GUI_PROPERTY_EDITOR_REGISTRY
