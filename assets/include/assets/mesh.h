#ifndef YAGA_ASSETS_MESH
#define YAGA_ASSETS_MESH

#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <vector>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "assets/vertex.h"
#include "utility/array.h"
#include "utility/update_notifier.h"

namespace yaga {
namespace assets {

class Mesh;
typedef std::unique_ptr<Mesh> MeshPtr;

enum class MeshProperty
{
  vertices,
  indices
};

class Mesh
  : public Asset
  , public UpdateNotifier<MeshProperty>
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
  void vertices(const Vertices& vertices);
  const Indices& indices() const { return indices_; }
  void indices(const Indices& indices);
  void indices(IndexUpdater handler);

public:
  static const SerializationInfo serializationInfo;
  static MeshPtr deserializeBinary(const std::string& name, std::istream& stream, size_t size, RefResolver& resolver);
  static MeshPtr deserializeFriendly(const std::string& name, const std::string& path, RefResolver& resolver);

private:
  Vertices vertices_;
  Indices indices_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_MESH
