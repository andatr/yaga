#include "precompiled.h"
#include "triangulate.h"

namespace yaga {
namespace triangulate {
namespace {

constexpr int capSlices = 12;

// -----------------------------------------------------------------------------------------------------------------------------
inline bool checkArguments(size_t points, size_t& from, size_t& to)
{
  if (from >= points)
    from = 0;
  if (to > points)
    to = points;
  return (points > 1) && (from < to);
}

// -----------------------------------------------------------------------------------------------------------------------------
inline float getPolyLength(glm::vec3* points, size_t pointsCount, bool cycle, float feathering)
{
  float result = 0.0f;
  for (size_t i = 0; i < pointsCount - 1; ++i) {
    result += glm::length(points[i + 1] - points[i]);
  }
  if (cycle) {
    result += glm::length(points[0] - points[pointsCount - 1]);
    result += 2.0f * feathering;
  }
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void updateVertex(
  size_t  points,
  Vertex* vertices,
  size_t  index,
  const glm::vec3& pos,
  const glm::vec3& prev,
  const glm::vec3& next,
  float offset,
  float featherOffset,
  float textureU,
  float textureV,
  const glm::vec4& color,
  const glm::vec4& featherColor)
{
  const glm::vec4 pos_  = glm::vec4(pos, 1.0f);
  // right
  size_t i = index;
  vertices[i].position  = pos_;
  vertices[i].normal    = glm::vec4(next, featherOffset);
  vertices[i].tangent   = glm::vec4(prev, offset);
  vertices[i].texture   = glm::vec2(textureU, 1.0f - textureV);
  vertices[i].color     = color;
  // left
  ++i;
  vertices[i].position  = pos_;
  vertices[i].normal    = glm::vec4(prev, featherOffset);
  vertices[i].tangent   = glm::vec4(next, offset);
  vertices[i].texture   = glm::vec2(textureU, textureV);
  vertices[i].color     = color;
  // feathering right
  i = index == 4 * points ? i + 1 : points * 2 + index;
  vertices[i].position  = pos_;
  vertices[i].normal    = glm::vec4(next, offset);
  vertices[i].tangent   = glm::vec4(prev, featherOffset);
  vertices[i].texture   = glm::vec2(textureU, 1.0f);
  vertices[i].color     = featherColor;
  // feathering left
  ++i;
  vertices[i].position  = pos_;
  vertices[i].normal    = glm::vec4(prev, offset);
  vertices[i].tangent   = glm::vec4(next, featherOffset);
  vertices[i].texture   = glm::vec2(textureU, 0.0f);
  vertices[i].color     = featherColor;
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void updateBluntEdge(
  Vertex* vertices,
  size_t  index,
  const glm::vec3& pos,
  const glm::vec3&,
  const glm::vec3& next,
  float offset,
  float featherOffset,
  float textureU,
  const glm::vec4& featherColor)
{
  const glm::vec4 pos_  = glm::vec4(pos, 2.0f);
  const glm::vec4 prev_ = glm::vec4(pos, offset);
  // right for start (left for end)
  vertices[index].position = pos_;
  vertices[index].normal   = glm::vec4(next, featherOffset);
  vertices[index].tangent  = prev_;
  vertices[index].texture  = glm::vec2(textureU, 1.0f - textureU);
  vertices[index].color    = featherColor;
  // left for start (right for end)
  ++index;
  vertices[index].position = pos_;
  vertices[index].normal   = glm::vec4(next, -featherOffset);
  vertices[index].tangent  = prev_;
  vertices[index].texture  = glm::vec2(textureU, textureU);
  vertices[index].color    = featherColor;
}

// -----------------------------------------------------------------------------------------------------------------------------
inline void updateCapEdge(
  Vertex* vertices,
  size_t  index,
  const glm::vec3& pos,
  const glm::vec3& prev,
  const glm::vec3& next,
  float feathering,
  float featherOffset,
  float textureU,
  float textureOffset,
  const glm::vec4& featherColor)
{
  using namespace boost::math::constants;
  const glm::vec4 pos_ = glm::vec4(pos, 1.0f);
  // central vertex
  vertices[index].position = pos_;
  vertices[index].normal   = glm::vec4(next, 0.0f);
  vertices[index].tangent  = glm::vec4(prev, 0.0f);
  vertices[index].texture  = glm::vec2(textureOffset, 0.5f);
  vertices[index].color    = featherColor;
  ++index;
  // edge vertices
  for (int i = 1; i < capSlices; ++i, ++index) {
    const float angle = pi<float>() * i / capSlices;
    const float sin_ = sinf(angle);
    const float cos_ = cosf(angle);
    vertices[index].position  = pos_;
    vertices[index].normal    = glm::vec4(next, featherOffset * cos_);
    vertices[index].tangent   = glm::vec4(prev, feathering * sin_);
    vertices[index].color     = featherColor;
    vertices[index].texture   = textureU < 0.5f
      ? glm::vec2(textureOffset - textureOffset * sin_, 0.5f + cos_ / 2.0f)
      : glm::vec2(textureOffset + (1.0f - textureOffset) * sin_, 0.5f - cos_ / 2.0f);
  }
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
void updateVertices(
  glm::vec3*         points,
  size_t             pointsCount,
  Vertex*            vertices,
  const VertexProps& props,
  EdgeType           edge,
  size_t             from,
  size_t             to)
{
  if (!checkArguments(pointsCount, from, to)) return;
  const glm::vec4 featherColor { props.color.x, props.color.y, props.color.z, 0.0f };
  const bool   cycle           = edge == EdgeType::Cycle;
  const float  offset          = props.thickness / 2.0f;
  const float  featherOffset   = offset + props.feathering;
  const size_t last            = pointsCount - 1;
  const float  polyLen         = getPolyLength(points, pointsCount, cycle, props.feathering);
  // because feathering is done in screen space, we don't know the real polyline length (with feathering) until we draw it
  // so value of "props.feathering / polyLen" is not too accurate, but should be ok
  const float  textureFeatherV = props.feathering / (props.thickness + 2.0f * props.feathering);
  const float  textureOffset   = cycle ? 0.0f : props.feathering / polyLen;
  const float  textureLength   = cycle ? 1.0f : (1.0f - 2.0f * props.feathering / polyLen);
  // the first vertex is special
  if (from == 0) {
    const glm::vec3 prev = cycle ? points[last] : (2.0f * points[0] - points[1]);
    updateVertex(
      pointsCount, vertices, 0,
      points[0], prev, points[1],
      offset, featherOffset,
      textureOffset, textureFeatherV,
      props.color, featherColor);
  }
  // middle vertices
  for (size_t i = std::max(1ull, from); i < std::min(last, to); ++i) {
    updateVertex(
      pointsCount, vertices, i * 2,
      points[i], points[i - 1], points[i + 1],
      offset, featherOffset,
      textureOffset + textureLength * i / (pointsCount - 1), textureFeatherV,
      props.color, featherColor);
  }
  // the last vertex is also special
  if (to == pointsCount) {
    const glm::vec3 next = cycle ? points[0] : (2.0f * points[last] - points[last - 1]);
    updateVertex(
      pointsCount, vertices, pointsCount * 2 - 2,
      points[last], points[last - 1], next,
      offset, featherOffset,
      textureOffset + textureLength, textureFeatherV,
      props.color, featherColor);
  }
  // additional vertices on edges
  if (edge == EdgeType::Blunt) {
    if (from == 0) {
      updateBluntEdge(
        vertices, pointsCount * 4,
        points[0], points[0], points[1],
        props.feathering, featherOffset,
        0.0f,
        featherColor);
    }
    if (to == pointsCount) {
      updateBluntEdge(
        vertices, pointsCount * 4 + 2,
        points[last], points[last], points[last - 1],
        props.feathering, featherOffset,
        1.0f,
        featherColor);
    }
  }
  else if (edge == EdgeType::Cap) {
    if (from == 0) {
      updateCapEdge(
        vertices, pointsCount * 4,
        points[0], points[0], points[1],
        props.feathering, featherOffset,
        0.0f, textureOffset,
        featherColor);
    }
    if (to == pointsCount) {
      updateCapEdge(
        vertices, pointsCount * 4 + capSlices,
        points[last], points[last], points[last - 1],
        props.feathering, featherOffset,
        1.0f, 1.0f - textureOffset,
        featherColor);
    }
  }
  else if (edge == EdgeType::Cycle) {
    // duplicate the first vertex with different texture coordinates
    updateVertex(
      pointsCount, vertices, pointsCount * 4,
      points[0], points[last], points[1],
      offset, featherOffset,
      1.0f, textureFeatherV,
      props.color, featherColor);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void updateIndices(glm::vec3*, size_t pointsCount, Index* indices, EdgeType edge, Index offset, size_t from, size_t to)
{
  if (!checkArguments(pointsCount, from, to)) return;
  const bool cycle = edge == EdgeType::Cycle;
  size_t index = 0;
  const Index count2 = static_cast<Index>(offset + pointsCount * 2);
  const Index count4 = static_cast<Index>(offset + pointsCount * 4);
  for (size_t i = from; i < std::min(pointsCount - 1, to); ++i) {
    // body
    const Index bodyVertex = static_cast<Index>(offset + i * 2);
    index = i * 6;
    indices[index + 0] = bodyVertex + 0;
    indices[index + 1] = bodyVertex + 2;
    indices[index + 2] = bodyVertex + 1;
    indices[index + 3] = bodyVertex + 2;
    indices[index + 4] = bodyVertex + 3;
    indices[index + 5] = bodyVertex + 1;
    // right feathering
    const Index offsetVertex = static_cast<Index>(offset + pointsCount * 2 + i * 2);
    index = i * 6 + pointsCount * 6 - (cycle ? 0 : 6);
    indices[index + 0] = bodyVertex   + 0;
    indices[index + 1] = offsetVertex + 0;
    indices[index + 2] = offsetVertex + 2;
    indices[index + 3] = offsetVertex + 2;
    indices[index + 4] = bodyVertex   + 2;
    indices[index + 5] = bodyVertex   + 0;
    // left feathering 
    index = i * 6 + pointsCount * 12 - (cycle ? 0 : 12);
    indices[index + 0] = bodyVertex   + 1;
    indices[index + 1] = bodyVertex   + 3;
    indices[index + 2] = offsetVertex + 1;
    indices[index + 3] = bodyVertex   + 3;
    indices[index + 4] = offsetVertex + 3;
    indices[index + 5] = offsetVertex + 1;
  }
  if (edge == EdgeType::Blunt) {
    // start cap
    if (from == 0) {
      index = (pointsCount - 1) * 18;
      const Index bodyVertex = static_cast<Index>(offset + pointsCount * 4);
      indices[index + 0] = count2;
      indices[index + 1] = bodyVertex + 1;
      indices[index + 2] = bodyVertex;
      indices[index + 3] = count2;
      indices[index + 4] = count2 + 1;
      indices[index + 5] = bodyVertex + 1;
    }
    // end cap
    if (to == pointsCount) {
      index = (pointsCount - 1) * 18 + 6;
      const Index bodyVertex = static_cast<Index>(offset + pointsCount * 4);
      indices[index + 0] = count4 - 2;
      indices[index + 1] = bodyVertex + 3;
      indices[index + 2] = bodyVertex + 2;
      indices[index + 3] = bodyVertex + 2;
      indices[index + 4] = count4 - 1;
      indices[index + 5] = count4 - 2;
    }
  }
  else if (edge == EdgeType::Cap) {
    // start cap
    if (from == 0) {
      index = (pointsCount - 1) * 18;
      indices[index + 0] = count4;
      indices[index + 1] = count4 + 1;
      indices[index + 2] = count2;
      index += 3;
      for (int i = 0; i < capSlices - 2; ++i, index += 3) {
        indices[index + 0] = count4 + 1 + i;
        indices[index + 1] = count4;
        indices[index + 2] = count4 + 2 + i;
      }
      indices[index + 0] = count4;
      indices[index + 1] = count2 + 1;
      indices[index + 2] = count4 + capSlices - 1;
    }
    // end cap
    if (to == pointsCount) {
      index = (pointsCount - 1) * 18 + capSlices * 3;
      const Index bodyVertex = static_cast<Index>(offset + pointsCount * 4 + capSlices);
      indices[index + 0] = bodyVertex;
      indices[index + 1] = bodyVertex + 1;
      indices[index + 2] = count4 - 1;
      index += 3;
      for (int i = 0; i < capSlices - 2; ++i, index += 3) {
        indices[index + 0] = bodyVertex;
        indices[index + 1] = bodyVertex + 2 + i;
        indices[index + 2] = bodyVertex + 1 + i;
      }
      indices[index + 0] = bodyVertex;
      indices[index + 1] = count4 - 2;
      indices[index + 2] = bodyVertex + capSlices - 1;
    }
  }
  else if (cycle) {
    if (from == 0 || to == pointsCount) {
      // body
      const Index bodyVertex = static_cast<Index>(offset + pointsCount * 2 - 2);
      index = pointsCount * 6 - 6;
      indices[index + 0] = bodyVertex;
      indices[index + 1] = count4;
      indices[index + 2] = bodyVertex + 1;
      indices[index + 3] = count4;
      indices[index + 4] = count4 + 1;
      indices[index + 5] = bodyVertex + 1;
      // right feathering
      index = pointsCount * 12 - 6;
      indices[index + 0] = bodyVertex;
      indices[index + 1] = count4 - 2;
      indices[index + 2] = count4 + 2;
      indices[index + 3] = count4 + 2;
      indices[index + 4] = count4;
      indices[index + 5] = bodyVertex;
      // left feathering 
      index = pointsCount * 18 - 6;
      indices[index + 0] = bodyVertex + 1;
      indices[index + 1] = count4 + 1;
      indices[index + 2] = count4 - 1;
      indices[index + 3] = count4 + 1;
      indices[index + 4] = count4 + 3;
      indices[index + 5] = count4 - 1;
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
VertexProps::VertexProps() :
  thickness(1.0f),
  feathering(1.0f),
  color{ 1.0f, 1.0f, 1.0f, 1.0f }
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::MeshPtr createMesh(const std::string& name, glm::vec3* points, size_t pointsCount,
  const VertexProps& props, EdgeType edge)
{
  auto mesh = std::make_unique<assets::Mesh>(name);
  mesh->vertices([points, pointsCount, &props, edge](auto& vertices) {
    vertices.resize(getVertexCount(pointsCount, edge));
    updateVertices(points, pointsCount, vertices.data(), props, edge);
  });
  mesh->indices([points, pointsCount, edge](auto& indices) {
    indices.resize(getIndexCount(pointsCount, edge));
    updateIndices(points, pointsCount, indices.data(), edge);
  });
  return std::move(mesh);
}

// -----------------------------------------------------------------------------------------------------------------------------
size_t getVertexCount(size_t pointsCount, EdgeType edge)
{
  if (edge == EdgeType::Blunt) {
    return pointsCount * 4 + 4;
  }
  else if (edge == EdgeType::Cap) {
    return pointsCount * 4 + 2 * capSlices;
  }
  else if (edge == EdgeType::Cycle) {
    return pointsCount * 4 + 4;
  }
  THROW("Unexpected triangulate::EdgeType value");
}

// -----------------------------------------------------------------------------------------------------------------------------
size_t getIndexCount(size_t pointsCount, EdgeType edge)
{
  if (edge == EdgeType::Blunt) {
    return (pointsCount - 1) * 18 + 12;
  }
  else if (edge == EdgeType::Cap) {
    return (pointsCount - 1) * 18 + capSlices * 6;
  }
  else if (edge == EdgeType::Cycle) {
    return pointsCount * 18;
  }
  THROW("Unexpected triangulate::EdgeType value");
}

} // !namespace triangulate
} // !namespace yaga
