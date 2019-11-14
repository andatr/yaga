#ifndef YAGA_UTILITY_AUTO_DESTROYER
#define YAGA_UTILITY_AUTO_DESTROYER

#include <functional>

#include <boost/core/noncopyable.hpp>

#include "exception.h"

namespace yaga
{

template<typename T>
class AutoDestroyer : private boost::noncopyable
{
public:
  typedef std::function<void(T)> DestructorT;
public:
  AutoDestroyer();
  AutoDestroyer(T& obj, const DestructorT& dtor);
  AutoDestroyer(AutoDestroyer<T>&& other) noexcept;
  AutoDestroyer<T>& operator=(AutoDestroyer<T>&& other) noexcept;
  ~AutoDestroyer();
  void Assign(T& obj, const DestructorT& dtor);
  void Reset();
  const T& operator*() const;
  bool Destoyed() const { return destoyed_; }
private:
  bool destoyed_;
  T object_;
  DestructorT destructor_;
};

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
AutoDestroyer<T>::AutoDestroyer():
  destoyed_(true)
{
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
AutoDestroyer<T>::AutoDestroyer(T& obj, const DestructorT& dtor):
  destoyed_(true)
{
  Assign(obj, dtor);
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
AutoDestroyer<T>::AutoDestroyer(AutoDestroyer<T>&& other) noexcept
{
  destoyed_ = other.destoyed_;
  destructor_ = other.destructor_;
  object_ = std::move(other.object_);
  other.destoyed_ = true;
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
AutoDestroyer<T>& AutoDestroyer<T>::operator=(AutoDestroyer<T>&& other) noexcept
{
  Reset();
  destoyed_ = other.destoyed_;
  destructor_ = other.destructor_;
  object_ = std::move(other.object_);
  other.destoyed_ = true;
  return *this;
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void AutoDestroyer<T>::Assign(T& obj, const DestructorT& dtor)
{
  Reset();
  destructor_ = dtor;
  object_ = std::move(obj);
  destoyed_ = false;
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
AutoDestroyer<T>::~AutoDestroyer()
{
  Reset();
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void AutoDestroyer<T>::Reset()
{
  if (destoyed_) return;
  destoyed_ = true;
  destructor_(object_);
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
const T& AutoDestroyer<T>::operator*() const
{
  if (destoyed_) {
    THROW("AutoDestroyer: attempt to access destoyed object");
  }
  return object_;
}

} // !namespace yaga

#endif // !YAGA_UTILITY_AUTO_DESTROYER