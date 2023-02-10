#include "precompiled.h"
#include "gui/property_editor_registry.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
PropertyEditorRegistry& PropertyEditorRegistry::instance()
{
  static PropertyEditorRegistry inst;
  return inst;
}

// -----------------------------------------------------------------------------------------------------------------------------
PropertyEditorPtr PropertyEditorRegistry::createEditor(IProperty* property)
{
  auto it = factories_.find(property->type());
  if (it == factories_.end()) {
    return nullptr;
  }
  return it->second(property);
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyEditorRegistry::registerFactory(std::type_index type, Factory factory)
{
  factories_[type] = factory;
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyEditorRegistry::hideProperty(std::type_index type, const std::string& name)
{
  auto it = hidden_.find(type);
  if (it == hidden_.end()) {
    hidden_[type] = { name };
  }
  else {
    it->second.insert(name);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
bool PropertyEditorRegistry::hidden(std::type_index type, const std::string& name)
{
  auto it = hidden_.find(type);
  if (it == hidden_.end()) return false;
  return it->second.find(name) != it->second.end();
}

} // !namespace yaga
