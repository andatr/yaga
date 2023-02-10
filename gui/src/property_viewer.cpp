#include "precompiled.h"
#include "gui/property_viewer.h"
#include "gui/property_editor.h"
#include "gui/property_editor_registry.h"
#include "gui/property_editor_standard.h"
#include "gui/widget.h"
#include "assets/transform.h"

namespace yaga {
namespace {

const ImVec2 defaultSize(300.0f, 300.0f);

struct PropertyHideRegistration
{
  PropertyHideRegistration();
};

PropertyHideRegistration hideReg;

// -----------------------------------------------------------------------------------------------------------------------------
PropertyHideRegistration::PropertyHideRegistration()
{
  auto& inst = PropertyEditorRegistry::instance();
  inst.hideProperty(typeid(assets::Transform), "Parent");
}

} // !namespace

PROP_EDITOR_REG(PropertyEditorCategory)
PROP_EDITOR_REG(StringPropertyEditor)
PROP_EDITOR_REG(IntPropertyEditor)
PROP_EDITOR_REG(Vec3PropertyEditor)
PROP_EDITOR_REG(QuatPropertyEditor)
PROP_EDITOR_REG(MeshBoundsPropertyEditor)

class PropertyViewerImpl : public Widget
{
public:
  explicit PropertyViewerImpl(GuiContext context);
  void target(PropInfo* value);
  void render() override;

private:
  PropInfo* target_;
  std::unique_ptr<PropertyEditorCategory> editor_;
};

// -----------------------------------------------------------------------------------------------------------------------------
PropertyViewerImpl::PropertyViewerImpl(GuiContext context) :
  Widget(context),
  target_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyViewerImpl::target(PropInfo* value)
{
  target_ = value;
  editor_ = nullptr;
  if (value == nullptr) return;
  editor_ = std::make_unique<PropertyEditorCategory>(value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyViewerImpl::render()
{
  ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
  setPosition();
  if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse) && editor_) {
    editor_->render();
  }
  updatePosition();
  ImGui::End();
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
PropertyViewer::PropertyViewer(GuiContext context)
{
  impl_ = std::make_shared<PropertyViewerImpl>(context);
  impl_->enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyViewer::target(PropInfo* value)
{
  impl_->target(value);
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyViewer::docking(Docking value)
{
  return impl_->docking(value);
}

} // !namespace yaga
