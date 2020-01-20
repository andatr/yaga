#ifndef YAGA_ENGINE_ASSET_MESH
#define YAGA_ENGINE_ASSET_MESH

#include <memory>
#include <istream>
#include <ostream>
#include <vector>

#include "engine/vertex.h"
#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"
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
public:
  static const SerializationInfo serializationInfo;
  static MeshPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static MeshPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_MESH
