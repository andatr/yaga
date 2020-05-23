#ifndef YAGA_ENGINE_ASSET_MESH
#define YAGA_ENGINE_ASSET_MESH

#include <functional>
#include <memory>
#include <istream>
#include <ostream>
#include <vector>

#include "engine/vertex.h"
#include "engine/asset/asset.h"
#include "engine/asset/serializer.h"
#include "utility/array.h"
#include "utility/update_notifier.h"

namespace yaga
{
namespace asset
{

class Mesh;
typedef std::unique_ptr<Mesh> MeshPtr;

enum class MeshProperty
{
  vertices,
  indices
};

class Mesh : public Asset, public UpdateNotifier<MeshProperty>
{
public:
  typedef std::vector<Vertex> Vertices;
  typedef std::vector<uint32_t> Indices;
  typedef std::function<void(Vertices&)> VertexUpdater;
  typedef std::function<void(Indices&)> IndexUpdater;
public:
  explicit Mesh(const std::string& name);
  virtual ~Mesh();
  const Vertices& vertices() const { return vertices_; }
  void vertices(VertexUpdater handler);
  void vertices(const Vertices& vertices) { vertices_ = vertices; }
  const Indices& indices() const { return indices_; }
  void indices(const Indices& indices) { indices_ = indices; }
  void indices(IndexUpdater handler);
public:
  static const SerializationInfo serializationInfo;
  static MeshPtr deserialize(const std::string& name, std::istream& stream, size_t size);
  static MeshPtr deserializeFriendly(const std::string& type, const std::string& name, std::istream& stream, size_t size);
private:
  Vertices vertices_;
  Indices indices_;
};

} // !namespace asset
} // !namespace yaga

#endif // !YAGA_ENGINE_ASSET_MESH
