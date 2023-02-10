#include "precompiled.h"
#include "assets/mesh.h"
#include "assets/storage.h"
#include "binary_serializer_helper.h"
#include "binary_serializer_registry.h"

namespace yaga {
namespace assets {

BINARY_SERIALIZER_REG(Mesh)

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::Mesh(const std::string& name) :
  Asset(name),
  bounds_{}
{
  addProperty<int64_t, size_t>("Vertices", std::bind(&Vertices::size, &vertices_));
  addProperty<int64_t, size_t>("Indices",  std::bind(&Indices::size,  &indices_ ));
  addProperty("Bounds", &bounds_, true);
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh* Mesh::vertices(VertexUpdater handler)
{
  handler(vertices_);
  properties_[PropertyIndex::vertices]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh* Mesh::vertices(const Vertices& value)
{
  vertices_ = value;
  properties_[PropertyIndex::vertices]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh* Mesh::indices(IndexUpdater handler)
{
  handler(indices_);
  properties_[PropertyIndex::indices]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh* Mesh::indices(const Indices& value)
{
  indices_ = value;
  properties_[PropertyIndex::indices]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
Mesh* Mesh::bounds(const Bounds& value)
{
  bounds_ = value;
  properties_[PropertyIndex::bounds]->update(this);
  return this;
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPtr Mesh::deserializeBinary(std::istream& stream)
{
  std::string name;
  binser::read(stream, name);
  auto mesh = std::make_unique<Mesh>(name);
  binser::read(stream, mesh->name_);
  binser::read(stream, mesh->bounds_.min);
  binser::read(stream, mesh->bounds_.max);
  binser::read(stream, mesh->vertices_);
  binser::read(stream, mesh->indices_);
  return mesh;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Mesh::serializeBinary(Asset* asset, std::ostream& stream)
{
  auto mesh = assetCast<Mesh>(asset);
  binser::write(stream, mesh->name_);
  binser::write(stream, mesh->bounds_.min); 
  binser::write(stream, mesh->bounds_.max);
  binser::write(stream, mesh->vertices_);
  binser::write(stream, mesh->indices_);
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshPtr Mesh::deserializeFriendly(std::istream&)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Mesh::serializeFriendly(Asset*, std::ostream&)
{
  THROW_NOT_IMPLEMENTED;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Mesh::resolveRefs(Asset*, Storage*)
{
}

} // !namespace assets
} // !namespace yaga
