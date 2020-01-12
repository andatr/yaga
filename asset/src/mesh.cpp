#include "precompiled.h"
#include "mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

namespace yaga
{
namespace asset
{

const AssetId Mesh::assetId = { "mesh", 2 };

// -------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(const std::string& name) :
  Asset(name)
{
}
  
// -------------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
}

// -------------------------------------------------------------------------------------------------------------------------
size_t Mesh::serialize(Asset* asset, std::ostream&, bool)
{
  auto mesh = dynamic_cast<Mesh*>(asset);
  if (!mesh) {
    THROW("Mesh serializer was given the wrong asset");
  }
  return 0;
}

// -------------------------------------------------------------------------------------------------------------------------
MeshPtr Mesh::deserialize(const std::string& name, std::istream&, size_t, bool)
{
  auto mesh = std::make_unique<Mesh>(name);
  /*mesh->vertices_ = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
  };
  mesh->indices_ = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
  };*/

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, R"(D:\Projects\Cpp\VulkanGameBuild3\launcher\Release\data\chalet.obj)")) {
    throw std::runtime_error(warn + err);
  }

  std::unordered_map<Vertex, uint32_t, std::hash<Vertex>> uniqueVertices = {};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex = {};

      vertex.pos = {
        attrib.vertices[3 * index.vertex_index + 0],
        attrib.vertices[3 * index.vertex_index + 1],
        attrib.vertices[3 * index.vertex_index + 2]
      };

      vertex.uv = {
        attrib.texcoords[2 * index.texcoord_index + 0],
        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
      };

      vertex.color = { 1.0f, 1.0f, 1.0f };

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices_.size());
        mesh->vertices_.push_back(vertex);
      }

      mesh->indices_.push_back(uniqueVertices[vertex]);
    }
  }


  return std::move(mesh);
}

} // !namespace asset
} // !namespace yaga
