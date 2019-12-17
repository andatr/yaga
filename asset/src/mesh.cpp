#include "precompiled.h"
#include "mesh.h"

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
size_t Mesh::Serialize(Asset* asset, std::ostream&, bool)
{
  auto mesh = dynamic_cast<Mesh*>(asset);
  if (!mesh) {
    THROW("Mesh serializer was given the wrong asset");
  }
  return 0;
}

// -------------------------------------------------------------------------------------------------------------------------
MeshPtr Mesh::Deserialize(const std::string& name, std::istream&, size_t, bool)
{
  auto mesh = std::make_unique<Mesh>(name);
  mesh->vertices_ = {
    { { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
    { { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
    { { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
    { { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
  };
  mesh->indices_ = {
    0, 1, 2, 2, 3, 0
  };
  return std::move(mesh);
}

} // !namespace asset
} // !namespace yaga
