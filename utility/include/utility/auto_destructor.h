#ifndef YAGA_UTILITY_AUTO_DESTROYER
#define YAGA_UTILITY_AUTO_DESTROYER

#include "utility/compiler.h"

#include <functional>

DISABLE_WARNINGS
#include <boost/noncopyable.hpp>
ENABLE_WARNINGS

#include "exception.h"

namespace yaga {

template <typename T>
class AutoDestructor : private boost::noncopyable
{
public:
  typedef std::function<void(T)> DestructorT;

public:
  AutoDestructor();
  AutoDestructor(T&  obj, const DestructorT& dtor);
  AutoDestructor(T&& obj, const DestructorT& dtor);
  AutoDestructor(AutoDestructor<T>&& other) noexcept;
  AutoDestructor<T>& operator=(AutoDestructor<T>&& other) noexcept;
  ~AutoDestructor();
  void set(T& obj, const DestructorT& dtor);
  void set();
  const T& operator*() const;
  const T& operator->() const { return operator*(); }
  bool destoyed() const { return destoyed_; }

private:
  bool destoyed_;
  T object_;
  DestructorT destructor_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
AutoDestructor<T>::AutoDestructor() :
  destoyed_(true),
  object_{}
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
AutoDestructor<T>::AutoDestructor(T& obj, const DestructorT& dtor) :
  destoyed_(true)
{
  set(obj, dtor);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
AutoDestructor<T>::AutoDestructor(T&& obj, const DestructorT& dtor) :
  destoyed_(true)
{
  set(obj, dtor);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
AutoDestructor<T>::AutoDestructor(AutoDestructor<T>&& other) noexcept
{
  destoyed_ = other.destoyed_;
  destructor_ = other.destructor_;
  object_ = std::move(other.object_);
  other.destoyed_ = true;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
AutoDestructor<T>& AutoDestructor<T>::operator=(AutoDestructor<T>&& other) noexcept
{
  set();
  destoyed_ = other.destoyed_;
  destructor_ = other.destructor_;
  object_ = std::move(other.object_);
  other.destoyed_ = true;
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void AutoDestructor<T>::set(T& obj, const DestructorT& dtor)
{
  set();
  destructor_ = dtor;
  object_ = std::move(obj);
  destoyed_ = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void AutoDestructor<T>::set()
{
  if (destoyed_) return;
  destoyed_ = true;
  destructor_(object_);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
AutoDestructor<T>::~AutoDestructor()
{
  set();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
const T& AutoDestructor<T>::operator*() const
{
#ifndef NDEBUG
  if (destoyed_) {
    THROW("AutoDestructor: attempt to access destoyed object");
  }
#endif
  return object_;
}

} // !namespace yaga

#endif // !YAGA_UTILITY_AUTO_DESTROYER