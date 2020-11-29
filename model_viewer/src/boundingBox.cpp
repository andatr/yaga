#include "precompiled.h"
#include "boundingBox.h"
#include "engine/triangulate.h"

namespace yaga {
namespace {

// axis
constexpr float axisLen         = 1.0f;
constexpr float axisWidth       = 0.1f;
constexpr float axisFeathering  = 0.02f;
// arrow
constexpr float arrowBackLen    = 0.1f;
constexpr float arrowHeadLen    = 0.4f;
constexpr float arrowWidth      = 0.8f;
constexpr float arrowPointWidth = 0.01f;
// bounding box
constexpr float bBoxWidth       = 0.05f;
constexpr float bBoxFeathering  = 0.01f;

constexpr glm::vec4 colors[4] = {
  { 0.7f, 0.0f, 0.0f, 1.0f }, // x
  { 0.0f, 0.7f, 0.0f, 1.0f }, // y
  { 0.0f, 0.0f, 0.7f, 1.0f }, // z
  { 1.0f, 0.0f, 1.0f, 1.0f }  // frame
};
constexpr float axisLen1 = axisLen + arrowBackLen;
constexpr float axisLen2 = axisLen1 + arrowHeadLen;

// -----------------------------------------------------------------------------------------------------------------------------
std::array<glm::vec3, 36> getPoints(const MeshMetadata& meta)
{
  const auto& min = meta.min;
  const auto& max = meta.max;
  const auto  mid = (max + min) / 2.0f;

  std::array<glm::vec3, 36> points;
  // x axis
  points[0]  = { max.x,            mid.y, mid.z };
  points[1]  = { max.x + axisLen,  mid.y, mid.z };
  points[2]  = { max.x + axisLen1, mid.y, mid.z };
  points[3]  = { max.x + axisLen2, mid.y, mid.z };
  // y axis
  points[4]  = { mid.x, max.y,            mid.z };
  points[5]  = { mid.x, max.y + axisLen,  mid.z };
  points[6]  = { mid.x, max.y + axisLen1, mid.z };
  points[7]  = { mid.x, max.y + axisLen2, mid.z };
  // z axis
  points[8]  = { mid.x, mid.y, max.z            };
  points[9]  = { mid.x, mid.y, max.z + axisLen  };
  points[10] = { mid.x, mid.y, max.z + axisLen1 };
  points[11] = { mid.x, mid.y, max.z + axisLen2 };
  // front pane
  points[12] = { min.x, min.y, max.z };
  points[13] = { min.x, max.y, max.z };
  points[14] = { max.x, max.y, max.z };
  points[15] = { max.x, min.y, max.z };
  // back pane
  points[16] = { min.x, min.y, min.z };
  points[17] = { min.x, max.y, min.z };
  points[18] = { max.x, max.y, min.z };
  points[19] = { max.x, min.y, min.z };
  // top pane
  points[20] = { min.x, max.y, min.z };
  points[21] = { min.x, max.y, max.z };
  points[22] = { max.x, max.y, max.z };
  points[23] = { max.x, max.y, min.z };
  // bottom pane
  points[24] = { min.x, min.y, min.z };
  points[25] = { min.x, min.y, max.z };
  points[26] = { max.x, min.y, max.z };
  points[27] = { max.x, min.y, min.z };
  // right pane
  points[28] = { max.x, min.y, min.z };
  points[29] = { max.x, min.y, max.z };
  points[30] = { max.x, max.y, max.z };
  points[31] = { max.x, max.y, min.z };
  // left pane
  points[32] = { min.x, min.y, min.z };
  points[33] = { min.x, min.y, max.z };
  points[34] = { min.x, max.y, max.z };
  points[35] = { min.x, max.y, min.z };
  return points;
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void makeAxis(glm::vec3* points, Vertex* vertices, const glm::vec4& color)
{
  // axis
  triangulate::VertexProps props;
  props.thickness = axisWidth;
  props.feathering = axisFeathering;
  props.color = color;
  triangulate::updateVertices(points, 4, vertices, props);
  // arrow start
  props.thickness = arrowWidth;
  triangulate::updateVertices(points, 4, vertices, props, triangulate::EdgeType::Blunt, 2, 3);
  // arrow head
  props.thickness = arrowPointWidth;
  triangulate::updateVertices(points, 4, vertices, props, triangulate::EdgeType::Blunt, 3, 4);
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
assets::MeshPtr makeBoundingBox(assets::Mesh* mesh, const MeshMetadata& meta)
{
  auto points = getPoints(meta);
  auto frameMesh = std::make_unique<assets::Mesh>(mesh->name() + "::frame");
  const auto axisVertices  = triangulate::getVertexCount(4);
  const auto axisIndices   = triangulate::getIndexCount (4);
  const auto frameVertices = triangulate::getVertexCount(4, triangulate::EdgeType::Cycle);
  const auto frameIndices  = triangulate::getIndexCount (4, triangulate::EdgeType::Cycle);
  frameMesh->vertices([&points, axisVertices, frameVertices](auto& vertices) {
    vertices.resize(axisVertices * 3 + frameVertices * 6);
    for (size_t i = 0; i < 3; ++i) {
      makeAxis(
        points.data() + 4 * i,
        vertices.data() + axisVertices * i,
        colors[i]);
    }
    triangulate::VertexProps props;
    props.thickness = bBoxWidth;
    props.feathering = bBoxFeathering;
    props.color = colors[3];
    for (size_t i = 3; i < 9; ++i) {
      triangulate::updateVertices(
        points.data() + 4 * i,
        4,
        vertices.data() + axisVertices * 3 + frameVertices * (i - 3),
        props,
        triangulate::EdgeType::Cycle);
    }
  });
  frameMesh->indices([&points, axisVertices, frameVertices, axisIndices, frameIndices](auto& indices) {
    indices.resize(axisIndices * 3 + frameIndices * 6);
    for (size_t i = 0; i < 3; ++i) {
      triangulate::updateIndices(
        points.data() + 4 * i,
        4,
        indices.data() + axisIndices * i,
        triangulate::EdgeType::Blunt,
        axisVertices * i);
    }
    for (size_t i = 3; i < 9; ++i) {
      triangulate::updateIndices(
        points.data() + 4 * i,
        4,
        indices.data() + axisIndices * 3 + frameIndices * (i - 3),
        triangulate::EdgeType::Cycle,
        axisVertices * 3 + frameVertices * (i - 3));
    }
  });
  return std::move(frameMesh);
}

} // !namespace yaga
