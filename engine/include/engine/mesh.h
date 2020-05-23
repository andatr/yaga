#ifndef YAGA_ENGINE_MESH
#define YAGA_ENGINE_MESH

#include <memory>

#include "component.h"
#include "engine/asset/mesh.h"

namespace yaga
{

class Mesh : public Component
{
public:
  explicit Mesh(Object* object, asset::Mesh* asset);
  virtual ~Mesh() {}
protected:
  asset::Mesh* asset_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MESH
