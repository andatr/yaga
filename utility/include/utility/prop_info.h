#ifndef YAGA_UTILITY_PROP_INFO
#define YAGA_UTILITY_PROP_INFO

#include "utility/compiler.h"

#include <type_traits>
#include <vector>

DISABLE_WARNINGS
#include <boost/type_traits.hpp>
ENABLE_WARNINGS

#include "utility/property.h"

namespace yaga {

class PropInfo
{
public:
  typedef std::vector<PropertyPtr> PropertyList;

public:
  virtual ~PropInfo() {}
  virtual PropertyList& properties() { return properties_; }
  virtual IProperty* properties(size_t index) { return properties_[index].get(); }

protected:
  template <typename T, std::enable_if_t<!boost::is_base_and_derived<PropInfo, T>::value>* = nullptr>
  IProperty* addProperty(const std::string& name, T* value);

  template <typename T, std::enable_if_t<boost::is_base_and_derived<PropInfo, T>::value>* = nullptr>
  IProperty* addProperty(const std::string& name, T* object);
  
  template <typename T, std::enable_if_t<boost::is_base_and_derived<PropInfo, T>::value>* = nullptr>
  IProperty* addProperty(const std::string& name, std::shared_ptr<T> value);

  template <typename T, std::enable_if_t<!boost::is_base_and_derived<PropInfo, T>::value>* = nullptr>
  IProperty* addProperty(const std::string& name, std::shared_ptr<T> value);

  template <typename T>
  IProperty* addProperty(const std::string& name, T* value, bool readOnly);

  template <typename VT, typename FT>
  IProperty* addProperty(const std::string& name, typename ReadOnlyProperty<FT, VT>::Getter getter);

protected:
  PropertyList properties_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, std::enable_if_t<!boost::is_base_and_derived<PropInfo, T>::value>*>
IProperty* PropInfo::addProperty(const std::string& name, T* value)
{
  properties_.push_back(std::make_unique<Property<T, false>>(name, value));
  return properties_.back().get();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, std::enable_if_t<boost::is_base_and_derived<PropInfo, T>::value>*>
IProperty* PropInfo::addProperty(const std::string& name, T* object)
{
  properties_.push_back(std::make_unique<Property<PropInfo, true>>(name, object));
  return properties_.back().get();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, std::enable_if_t<!boost::is_base_and_derived<PropInfo, T>::value>*>
IProperty* PropInfo::addProperty(const std::string& name, std::shared_ptr<T> value)
{
  properties_.push_back(std::make_unique<Property<T, false>>(name, value.get()));
  return properties_.back().get();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, std::enable_if_t<boost::is_base_and_derived<PropInfo, T>::value>*>
IProperty* PropInfo::addProperty(const std::string& name, std::shared_ptr<T> object)
{
  properties_.push_back(std::make_unique<Property<PropInfo, true>>(name, object.get()));
  return properties_.back().get();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
IProperty* PropInfo::addProperty(const std::string& name, T* value, bool readOnly)
{
  if (readOnly) {
    properties_.push_back(std::make_unique<Property<T, true>>(name, value));
  }
  else {
    properties_.push_back(std::make_unique<Property<T, false>>(name, value));
  }
  return properties_.back().get();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename VT, typename FT>
IProperty* PropInfo::addProperty(const std::string& name, typename ReadOnlyProperty<FT, VT>::Getter getter)
{
  properties_.push_back(std::make_unique<ReadOnlyProperty<FT, VT>>(name, getter));
  return properties_.back().get();
}

} // !namespace yaga

#endif // !YAGA_UTILITY_PROP_INFO
