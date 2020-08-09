#include "precompiled.h"
#include "assets/mesh.h"

#pragma warning(push, 0)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#pragma warning(pop)

namespace std {

template <>
struct hash<yaga::Vertex>
{
  size_t operator()(yaga::Vertex const& vertex) const;
};

// -------------------------------------------------------------------------------------------------------------------------
size_t hash<yaga::Vertex>::operator()(yaga::Vertex const& vertex) const
{
  return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
}

} // !namespace std

namespace yaga {

// -------------------------------------------------------------------------------------------------------------------------
bool operator==(const Vertex& first, const Vertex& second)
{
  return first.pos == second.pos && first.color == second.color && first.uv == second.uv;
}

namespace assets {

const SerializationInfo Mesh::serializationInfo = { (uint32_t)StandardAssetId::mesh, { "obj" }, &Mesh::deserializeBinary,
  &Mesh::deserializeFriendly };

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(const std::string& name) : Asset(name) {}

// -------------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh() {}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::vertices(VertexUpdater handler)
{
  handler(vertices_);
  fireUpdate(MeshProperty::vertices);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::vertices(const Vertices& vertices)
{
  vertices_ = vertices;
  fireUpdate(MeshProperty::vertices);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::indices(IndexUpdater handler)
{
  handler(indices_);
  fireUpdate(MeshProperty::indices);
}

// -------------------------------------------------------------------------------------------------------------------------
void Mesh::indices(const Indices& indices)
{
  indices_ = indices;
  fireUpdate(MeshProperty::indices);
}

// -------------------------------------------------------------------------------------------------------------------------
MeshPtr Mesh::deserializeBinary(const std::string&, std::istream&, size_t, RefResolver&)
{
  THROW_NOT_IMPLEMENTED;
}

// -------------------------------------------------------------------------------------------------------------------------
MeshPtr Mesh::deserializeFriendly(const std::string& name, const std::string& path, RefResolver&)
{
  // TODO: rework!
  auto mesh = std::make_unique<Mesh>(name);

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;
  // std::ifstream stream(path, std::ios::in | std::ios::binary);
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
    throw std::runtime_error(err);
  }
  std::unordered_map<Vertex, uint32_t, std::hash<Vertex>> uniqueVertices{};
  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.pos = { attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
        attrib.vertices[3 * index.vertex_index + 2] };

      vertex.uv = { attrib.texcoords[2 * index.texcoord_index + 0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

      vertex.color = { 1.0f, 1.0f, 1.0f };

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices_.size());
        mesh->vertices_.push_back(vertex);
      }

      mesh->indices_.push_back(uniqueVertices[vertex]);
    }
  }
  return mesh;
}

} // !namespace assets
} // !namespace yaga
