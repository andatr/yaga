#ifndef YAGA_ASSETS_BINARY_SERIALIZER_HELPER
#define YAGA_ASSETS_BINARY_SERIALIZER_HELPER

#include "utility/compiler.h"

#include <istream>
#include <ostream>
#include <type_traits>
#include <vector>

#include "utility/glm.h"

#define THROW_ASSET_CAST(T) THROW(#T##" serializer got wrong asset");

namespace yaga   {
namespace assets {

template<typename T, typename R>
R* assetCast(T* value);

namespace binser {

void read (std::istream& stream,       std::string& value);
void write(std::ostream& stream, const std::string& value);
void read (std::istream& stream,       glm::vec3&   value);
void write(std::ostream& stream, const glm::vec3&   value);
void read (std::istream& stream,       glm::quat&   value);
void write(std::ostream& stream, const glm::quat&   value);
template<typename T>
void read(std::istream&  stream, T& value);
template<typename T>
void write(std::ostream& stream, const T& value);
template<typename T, bool>
void read(std::istream&  stream, std::vector<T>& value);
template<typename T, bool>
void write(std::ostream& stream, const std::vector<T>& value);

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void read(std::istream& stream, T& value)
{
  stream.read((char*)&value, sizeof(T));
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
void write(std::ostream& stream, const T& value)
{
  stream.write((char*)&value, sizeof(T));
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T, typename std::enable_if<std::is_standard_layout<T>::value, bool>::type = true>
void read(std::istream& stream, std::vector<T>& value)
{
  uint64_t size = 0;
  read(stream, size);
  value.resize(size);
  stream.read((char*)value.data(), sizeof(T) * size);
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T, typename std::enable_if<!std::is_standard_layout<T>::value, bool>::type = true>
void read(std::istream& stream, std::vector<T>& value)
{
  uint64_t size = 0;
  read(stream, size);
  value.resize(size);
  for (size_t i = 0; i < size; ++i) {
    read(stream, value[i]);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T, typename std::enable_if<std::is_standard_layout<T>::value, bool>::type = true>
void write(std::ostream& stream, const std::vector<T>& value)
{
  uint32_t size = static_cast<uint32_t>(value.size());
  write(stream, size);
  stream.write((char*)value.data(), size * sizeof(T));
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T, typename std::enable_if<!std::is_standard_layout<T>::value, bool>::type = true>
void write(std::ostream& stream, const std::vector<T>& value)
{
  uint32_t size = static_cast<uint32_t>(value.size());
  write(stream, size);
  for (size_t i = 0; i < size; ++i) {
    write(stream, value[i]);
  }
}

} // !namespace binser

// -----------------------------------------------------------------------------------------------------------------------------
template<typename R, typename T>
R* assetCast(T* value)
{
  auto result = dynamic_cast<R*>(value);
  if (!result) THROW_ASSET_CAST(R);
  return result;
}

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_BINARY_SERIALIZER_HELPER