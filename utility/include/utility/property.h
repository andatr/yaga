#ifndef YAGA_UTILITY_PROPERTY
#define YAGA_UTILITY_PROPERTY

#include <functional>
#include <string>
#include <typeindex> 

#include "utility/signal.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class IProperty
{
public:
  typedef boost::signals2::signal<void(void*)> SignalUpdate;

public:
  IProperty(const std::string& name, const std::type_index type, bool readOnly);
  virtual ~IProperty() {}
  const std::string&     name() const { return name_; }
  const std::type_index& type() const { return type_; }
  bool readOnly() { return readOnly_; }
  virtual void* get() = 0;
  virtual void set(void* value, void* sender) = 0;
  void update(void* sender) { sigUpdate_(sender); }
  SignalConnectionPtr onUpdate(const SignalUpdate::slot_type& handler);
  template <typename T>
  T* get() { return static_cast<T*>(get()); }
  template <typename T>
  T& get() { return *static_cast<T*>(get()); }

protected:
  std::string name_;
  const std::type_index type_;
  bool readOnly_;
  SignalUpdate sigUpdate_;
};

typedef std::unique_ptr<IProperty> PropertyPtr;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, bool R>
class Property : public IProperty
{
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
class Property<T, true> : public IProperty
{
public:
  Property(const std::string& name, T* value);
  void* get() override { return value_; }
  void set(void*, void*) override {}

private:
  T* value_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
class Property<T, false> : public IProperty
{
public:
  Property(const std::string& name, T* value);
  void* get() override { return value_; }
  void set(void* value, void* sender) override;

private:
  T* value_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename FT, typename VT>
class ReadOnlyProperty : public IProperty
{
public:
  typedef std::function<FT()> Getter;

public:
  ReadOnlyProperty(const std::string& name, Getter getter);
  void* get() override;
  void set(void*, void*) override {}

private:
  VT tmp_;
  Getter getter_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
Property<T, true>::Property(const std::string& name, T* value) :
  IProperty(name, typeid(T), true),
  value_(value)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
Property<T, false>::Property(const std::string& name, T* value) :
  IProperty(name, typeid(T), false),
  value_(value)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Property<T, false>::set(void* value, void* sender)
{
  *value_ = *static_cast<T*>(value);
  update(sender);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename FT, typename VT>
ReadOnlyProperty<FT, VT>::ReadOnlyProperty(const std::string& name, Getter getter) :
  IProperty(name, typeid(VT), true),
  getter_(getter)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename FT, typename VT>
void* ReadOnlyProperty<FT, VT>::get()
{
  tmp_ = static_cast<VT>(getter_());
  return &tmp_;
}

} // !namespace yaga

#endif // !YAGA_ASSETS_PROPERTY
