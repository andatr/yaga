#ifndef YAGA_IMGUI_BINDINGS_IMGUI_CONFIG
#define YAGA_IMGUI_BINDINGS_IMGUI_CONFIG

#pragma warning(push, 0)
#include <glm/glm.hpp>
#pragma warning(pop)

#define IM_VEC2_CLASS_EXTRA                                    \
                                                               \
ImVec2(const glm::uvec2& vec)                                  \
{                                                              \
  x = static_cast<float>(vec.x);                               \
  y = static_cast<float>(vec.y);                               \
}                                                              \
                                                               \
operator glm::uvec2() const                                    \
{                                                              \
  return glm::uvec2(                                           \
    static_cast<unsigned int>(x),                              \
    static_cast<unsigned int>(y)                               \
  );                                                           \
}                                                              \
                                                               \
ImVec2(const glm::vec2& vec)                                   \
{                                                              \
  x = vec.x;                                                   \
  y = vec.y;                                                   \
}                                                              \
                                                               \
operator glm::vec2() const                                     \
{                                                              \
  return glm::vec2(x, y);                                      \
}                                                              \
                                                               \
ImVec2& operator+=(const ImVec2& vec)                          \
{                                                              \
  x += vec.x;                                                  \
  y += vec.y;                                                  \
  return *this;                                                \
}                                                              \
                                                               \
ImVec2& operator-=(const ImVec2& vec)                          \
{                                                              \
  x -= vec.x;                                                  \
  y -= vec.y;                                                  \
  return *this;                                                \
}                                                              \
                                                               \
ImVec2& operator*=(float val)                                  \
{                                                              \
  x *= val;                                                    \
  y *= val;                                                    \
  return *this;                                                \
}                                                              \
                                                               \
ImVec2& operator/=(float val)                                  \
{                                                              \
  x /= val;                                                    \
  y /= val;                                                    \
  return *this;                                                \
}                                                              \
                                                               \
ImVec2 operator+(const ImVec2& vec) const                      \
{                                                              \
  return { x + vec.x, y + vec.y };                             \
}                                                              \
                                                               \
ImVec2 operator-(const ImVec2& vec) const                      \
{                                                              \
  return { x - vec.x, y - vec.y };                             \
}                                                              \
                                                               \
ImVec2 operator*(float val) const                              \
{                                                              \
  return { x * val, y * val };                                 \
}                                                              \
                                                               \
ImVec2 operator/(float val) const                              \
{                                                              \
  return { x / val, y / val };                                 \
}                                                              \

// !#define IM_VEC2_CLASS_EXTRA

#endif // !YAGA_IMGUI_BINDINGS_IMGUI_CONFIG