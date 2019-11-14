#ifndef YAGA_ASSET_MESH
#define YAGA_ASSET_MESH

#include <memory>
#include <istream>
#include <ostream>
#include <vector>

#include "asset.h"
#include "serializer.h"
#include "vertex.h"
#include "utility/array.h"

namespace yaga
{
namespace asset
{

class Mesh;
typedef std::unique_ptr<Mesh> MeshPtr;

class Mesh : public Asset
{
public:
  explicit Mesh(const std::string& name);
  virtual ~Mesh();
  const std::vector<Vertex>& Vertices() const { return vertices_; }
  Mesh& Vertices(const std::vector<Vertex>& vertices) { vertices_ = vertices; return *this; }
  const std::vector<uint16_t>& Indices() const { return indices_; }
  Mesh& Indices(const std::vector<uint16_t>& indices) { indices_ = indices; return *this; }
public: // Serialization
  static const AssetId assetId;
  static size_t Serialize(Asset* asset, std::ostream& stream, bool binary);
  static MeshPtr Deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
private:
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_MESH
