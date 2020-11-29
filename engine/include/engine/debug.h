#ifndef YAGA_ENGINE_DEBUG
#define YAGA_ENGINE_DEBUG

#include <ostream>

#include "glm.h"

namespace yaga {

// thanks ADL
template< typename T >
struct Printable
{
  T const& value;
};

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
Printable<T> toStream(T const& value) {
  return { value };
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream, const Printable<glm::vec2>& v)
{
  stream << "(" << v.value.x << ", " << v.value.y << ")";
  return stream;
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream, const Printable<glm::vec3>& v)
{
  stream << "(" << v.value.x << ", " << v.value.y << ", " << v.value.z << ")";
  return stream;
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream, const Printable<glm::vec4>& v)
{
  stream << "(" << v.value.x << ", " << v.value.y << ", " << v.value.z << ", " << v.value.w << ")";
  return stream;
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream, const Printable<glm::mat2>& m)
{
  stream << "(" << m.value[0].x << ", " << m.value[0].y << ")\n";
  stream << "(" << m.value[1].x << ", " << m.value[1].y << ")";
  return stream;
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream, const Printable<glm::mat3>& m)
{
  stream << "(" << m.value[0].x << ", " << m.value[0].y << ", " << m.value[0].z << ")\n";
  stream << "(" << m.value[1].x << ", " << m.value[1].y << ", " << m.value[1].z << ")\n";
  stream << "(" << m.value[2].x << ", " << m.value[2].y << ", " << m.value[2].z << ")";
  return stream;
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& stream, const Printable<glm::mat4>& m)
{
  stream << "(" << m.value[0].x << ", " << m.value[0].y << ", " << m.value[0].z << ", " << m.value[0].w << ")\n";
  stream << "(" << m.value[1].x << ", " << m.value[1].y << ", " << m.value[1].z << ", " << m.value[1].w << ")\n";
  stream << "(" << m.value[2].x << ", " << m.value[2].y << ", " << m.value[2].z << ", " << m.value[2].w << ")\n";
  stream << "(" << m.value[3].x << ", " << m.value[3].y << ", " << m.value[3].z << ", " << m.value[3].w << ")";
  return stream;
}

} // !namespace yaga

#endif // !YAGA_ENGINE_DEBUG
