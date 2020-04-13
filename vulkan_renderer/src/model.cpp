#include "precompiled.h"
#include "model.h"

namespace yaga
{
namespace vk
{

// -------------------------------------------------------------------------------------------------------------------------
Model::Model(Mesh* mesh, Material* material) :
  mesh_(mesh), material_(material)
{
}

} // !namespace vk
} // !namespace yaga