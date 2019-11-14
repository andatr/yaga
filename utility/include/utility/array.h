#ifndef YAGA_UTILITY_ARRAY
#define YAGA_UTILITY_ARRAY

#include <type_traits>

namespace yaga
{

template<typename T>
class Array
{
public:
  static_assert(std::is_pod<T>::value, "Array<T>: T must be POD");
public:
  explicit Array(size_t size = 0);
  void Resize(size_t size);
  T* operator*() const { return data_.get(); }
  T* Data() const { return data_.get(); }
  T& operator[](size_t i) { return data_.get()[i]; }
  size_t Size() const { return size_; }
private:
  static void Delete(const T* p);
private:
  size_t size_;
  std::shared_ptr<T> data_;
};

typedef Array<char> ByteArray;

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
Array<T>::Array(size_t size) : size_(size), data_(size > 0 ? new T[size] : nullptr, Delete)
{
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Array<T>::Resize(size_t size)
{
  size_ = size;
  data_ = std::shared_ptr<T>(size > 0 ? new T[size] : nullptr, Delete);
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Array<T>::Delete(const T* p)
{
  delete[] p;
}

} // !namespace yaga

#endif // !YAGA_UTILITY_ARRAY