#ifndef YAGA_UTILITY_ARRAY
#define YAGA_UTILITY_ARRAY

#include <type_traits>

namespace yaga
{

template<typename T>
class Array
{
public:
  static_assert(std::is_pod<T>::value, "yaga::Array<T>: T must be POD");
public:
  explicit Array(size_t size = 0);
  void resize(size_t size);
  T* operator*() const { return data_.get(); }
  T* data() const { return data_.get(); }
  T& operator[](size_t i) { return data_.get()[i]; }
  size_t size() const { return size_; }
private:
  static void free(const T* p);
private:
  size_t size_;
  std::shared_ptr<T> data_;
};

typedef Array<char> ByteArray;

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
Array<T>::Array(size_t size) : size_(size), data_(size > 0 ? new T[size] : nullptr, Array::free)
{
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Array<T>::resize(size_t size)
{
  auto newData = std::shared_ptr<T>(size > 0 ? new T[size] : nullptr, Array::free);
  if (size_ > 0 && size > 0) {
    memcpy((char*)newData.get(), (char*)data_.get(), sizeof(T) * std::min(size_, size));
  }
  size_ = size;
  data_ = newData;
}

// -------------------------------------------------------------------------------------------------------------------------
template<typename T>
void Array<T>::free(const T* p)
{
  delete[] p;
}

} // !namespace yaga

#endif // !YAGA_UTILITY_ARRAY