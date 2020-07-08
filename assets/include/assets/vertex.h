#ifndef YAGA_ASSETS_VERTEX
#define YAGA_ASSETS_VERTEX

#include "assets/glm.h"

namespace yaga {

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 uv;
};

} // !namespace yaga

#endif // !YAGA_ASSETS_VERTEX
