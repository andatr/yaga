#ifndef YAGA_ENGINE_TRIANGULATE
#define YAGA_ENGINE_TRIANGULATE

#include <functional>
#include <limits>

#include "assets/mesh.h"

namespace yaga {
namespace triangulate {

enum class EdgeType
{
  Cycle,
  Blunt, 
  Cap
};

enum Options : int
{
  AutoTexture = 1
};

struct VertexProps
{
  float thickness;
  float feathering;
  glm::vec4 color;

  VertexProps();
};

constexpr size_t BAD_INDEX = std::numeric_limits<size_t>::max();

assets::MeshPtr createMesh(
  const std::string& name,
  glm::vec3*         points,
  size_t             pointsCount,
  const VertexProps& props   = VertexProps(),
  EdgeType           edge    = EdgeType::Blunt);

void updateVertices(
  glm::vec3*         points,
  size_t             pointsCount,
  Vertex*            vertices,
  const VertexProps& props   = VertexProps(),
  EdgeType           edge    = EdgeType::Blunt,
  size_t             from    = BAD_INDEX,
  size_t             to      = BAD_INDEX);

void updateIndices(
  glm::vec3* points,
  size_t     pointsCount,
  Index*     indices,
  EdgeType   edge   = EdgeType::Blunt,
  Index      offset = 0,
  size_t     from   = BAD_INDEX,
  size_t     to     = BAD_INDEX);

size_t getVertexCount(
  size_t   pointsCount,
  EdgeType edge = EdgeType::Blunt);

size_t getIndexCount(
  size_t   pointsCount,
  EdgeType edge = EdgeType::Blunt);

} // !namespace triangulate
} // !namespace yaga

#endif // !YAGA_ENGINE_TRIANGULATE
