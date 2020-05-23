#ifndef YAGA_ENGINE_MATERIAL
#define YAGA_ENGINE_MATERIAL

#include <memory>

#include "component.h"
#include "engine/asset/material.h"

namespace yaga
{

class Material : public Component
{
public:
  explicit Material(Object* object, asset::Material* asset);
  virtual ~Material() {}
  asset::Material* asset() const { return asset_; }
protected:
  asset::Material* asset_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MATERIAL
