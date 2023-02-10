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
#include "utility/glm.h"

namespace yaga {
namespace assets {

class Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;

class Mesh : public Asset
{
public:
  struct PropertyIndex
  {
    static const int vertices = 0;
    static const int indices  = 1;
    static const int bounds   = 2;
  };

public:
  typedef std::vector<Vertex> Vertices;
  typedef std::vector<Index> Indices;
  typedef std::function<void(Vertices&)> VertexUpdater;
  typedef std::function<void(Indices&)>  IndexUpdater;
  struct Bounds
  {
    glm::vec3 min;
    glm::vec3 max;
  };

public:
  explicit Mesh(const std::string& name);
  virtual ~Mesh();
  const Vertices& vertices() const { return vertices_; }
  const Indices&  indices()  const { return indices_;  }
        Bounds&   bounds()         { return bounds_;   }
  Mesh* vertices(VertexUpdater   handler );
  Mesh* indices (IndexUpdater    handler );
  Mesh* vertices(const Vertices& value);
  Mesh* indices (const Indices&  value);
  Mesh* bounds  (const Bounds&   value);
  AssetType type() const override { return typeId; }

public:
  static const AssetType typeId = (uint32_t)StandardAssetType::mesh;
  static MeshPtr deserializeBinary  (std::istream& stream);
  static MeshPtr deserializeFriendly(std::istream& stream);
  static void serializeBinary  (Asset* asset, std::ostream& stream);
  static void serializeFriendly(Asset* asset, std::ostream& stream);
  static void resolveRefs(Asset* asset, Storage* storage);

private:
  Vertices vertices_;
  Indices indices_;
  Bounds bounds_;
};

} // !namespace assets
} // !namespace yaga

#endif // !YAGA_ASSETS_MESH
