#ifndef YAGA_ENGINE_MATERIAL
#define YAGA_ENGINE_MATERIAL

#include <memory>

#include "assets/material.h"
#include "engine/component.h"

namespace yaga {

class Material : public Component
{
public:
  explicit Material(Object* object, assets::Material* asset);
  virtual ~Material() {}
  assets::Material* asset() const { return asset_; }

protected:
  assets::Material* asset_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MATERIAL
