#ifndef YAGA_ENGINE_VERTEX
#define YAGA_ENGINE_VERTEX

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace yaga
{

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 uv;

  bool operator==(const Vertex& other) const;
};

struct UniformObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};

} // !namespace yaga

namespace std
{

template<>
struct hash<yaga::Vertex>
{
  size_t operator()(yaga::Vertex const& vertex) const;
};

} // !namespace std

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
inline bool Vertex::operator==(const Vertex& other) const {
  return pos == other.pos && color == other.color && uv == other.uv;
}

} // !namespace yaga

namespace std
{

// -------------------------------------------------------------------------------------------------------------------------
inline size_t hash<yaga::Vertex>::operator()(yaga::Vertex const& vertex) const
{
  return ((hash<glm::vec3>()(vertex.pos) ^
    (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
    (hash<glm::vec2>()(vertex.uv) << 1);
}

} // !namespace std

#endif // !YAGA_ENGINE_VERTEX
