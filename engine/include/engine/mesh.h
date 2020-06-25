#ifndef YAGA_ENGINE_MESH
#define YAGA_ENGINE_MESH

#include <memory>

#include "component.h"
#include "assets/mesh.h"

namespace yaga
{

class Mesh : public Component
{
public:
  explicit Mesh(Object* object, assets::Mesh* asset);
  virtual ~Mesh() {}
protected:
  assets::Mesh* asset_;
};

typedef std::unique_ptr<Mesh> MeshPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MESH
