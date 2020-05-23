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

} // !namespace yaga

#endif // !YAGA_ENGINE_VERTEX
