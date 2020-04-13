#ifndef YAGA_VULKAN_RENDERER_MODEL
#define YAGA_VULKAN_RENDERER_MODEL

#include <memory>

#include "material.h"
#include "mesh.h"
#include "engine/model.h"

namespace yaga
{
namespace vk
{

class Model : public yaga::Model
{
public:
  explicit Model(Mesh* mesh, Material* material);
  Mesh* mesh() const { return mesh_; }
  Material* material() const { return material_; }
private:
  Mesh* mesh_;
  Material* material_;
};

typedef std::unique_ptr<Model> ModelPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_MODEL
