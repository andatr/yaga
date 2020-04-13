#ifndef YAGA_ENGINE_VERTEX
#define YAGA_ENGINE_VERTEX

#include <glm/glm.hpp>

namespace yaga
{

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 uv;
};

struct UniformObject
{
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};

} // !namespace yaga

#endif // !YAGA_ENGINE_VERTEX
