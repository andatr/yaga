#ifndef YAGA_ASSETS_VERTEX
#define YAGA_ASSETS_VERTEX

#include "utility/glm.h"

namespace yaga {

struct Vertex
{
  glm::vec4 position;
  glm::vec4 normal;
  glm::vec4 tangent;
  glm::vec2 texture;
  glm::vec4 color;
};

typedef uint32_t Index;

} // !namespace yaga

#endif // !YAGA_ASSETS_VERTEX
