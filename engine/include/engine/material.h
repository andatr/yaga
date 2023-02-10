#ifndef YAGA_ENGINE_MATERIAL
#define YAGA_ENGINE_MATERIAL

#include <memory>

#include "assets/material.h"
#include "engine/component.h"

namespace yaga {

class Material : public Component
{
friend class Context;

public:
  virtual ~Material() {}
  assets::MaterialPtr asset() const { return asset_; }
  virtual bool wireframe() = 0;
  virtual void wireframe(bool value) = 0;
  const std::string& name() override;

protected:
  explicit Material(assets::MaterialPtr asset);

protected:
  assets::MaterialPtr asset_;
};

typedef std::unique_ptr<Material> MaterialPtr;

} // !namespace yaga

#endif // !YAGA_ENGINE_MATERIAL
