#include "precompiled.h"
#include "binary_serializer_helper.h"

namespace yaga   {
namespace assets {
namespace binser {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int size>
void readArray(std::istream& stream, const T& value)
{
  for (int i = 0; i < size; ++i) {
    stream.read((char*)&value[i], sizeof(value[i]));
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int size>
void writeArray(std::ostream& stream, const T& value)
{
  for (int i = 0; i < size; ++i) {
    stream.write((char*)&value[i], sizeof(value[i]));
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void read(std::istream& stream, std::string& value)
{
  uint32_t length = 0;
  stream.read((char*)&length, sizeof(uint32_t));
  std::vector<char> buffer(length);
  stream.read(buffer.data(), length);
  value.assign(buffer.begin(), buffer.end());
}

// -----------------------------------------------------------------------------------------------------------------------------
void write(std::ostream& stream, const std::string& value)
{
  uint32_t length = static_cast<uint32_t>(value.size());
  stream.write((char*)&length, sizeof(uint32_t));
  stream.write(value.c_str(), length);
}
  
// -----------------------------------------------------------------------------------------------------------------------------
void read(std::istream& stream, glm::vec3& value)
{
  readArray<glm::vec3, 3>(stream, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void write(std::ostream& stream, const glm::vec3& value)
{
  writeArray<glm::vec3, 3>(stream, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void read(std::istream& stream, glm::quat& value)
{
  readArray<glm::quat, 4>(stream, value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void write(std::ostream& stream, const glm::quat& value)
{
  writeArray<glm::quat, 4>(stream, value);
}

} // !namespace binser
} // !namespace assets
} // !namespace yaga