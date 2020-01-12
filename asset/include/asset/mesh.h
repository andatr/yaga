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
  const std::vector<Vertex>& vertices() const { return vertices_; }
  Mesh& vertices(const std::vector<Vertex>& vertices) { vertices_ = vertices; return *this; }
  const std::vector<uint32_t>& indices() const { return indices_; }
  Mesh& indices(const std::vector<uint32_t>& indices) { indices_ = indices; return *this; }
public: // Serialization
  static const AssetId assetId;
  static size_t serialize(Asset* asset, std::ostream& stream, bool binary);
  static MeshPtr deserialize(const std::string& name, std::istream& stream, size_t size, bool binary);
private:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ASSET_MESH
