#ifndef YAGA_ENGINE_MESH
#define YAGA_ENGINE_MESH

#include <memory>

#include "assets/mesh.h"
#include "engine/component.h"

namespace yaga {

class Mesh : public Component
{
friend class Context;

public:
  virtual ~Mesh() {}
  assets::MeshPtr asset() const { return asset_; }
  PropertyList& properties() override;
  const std::string& name() override;

protected:
  explicit Mesh(assets::MeshPtr asset);

protected:
  assets::MeshPtr asset_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MESH
