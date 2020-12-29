#ifndef YAGA_ENGINE_MESH
#define YAGA_ENGINE_MESH

#include <memory>

#include "assets/mesh.h"
#include "engine/component.h"

namespace yaga {

class Mesh : public Component
{
public:
  explicit Mesh(Object* object, assets::Mesh* asset);
  virtual ~Mesh() {}
  assets::Mesh* asset() const { return asset_; }

protected:
  assets::Mesh* asset_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MESH
