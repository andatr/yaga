#ifndef YAGA_GUI_PROPERTY_VIEWER
#define YAGA_GUI_PROPERTY_VIEWER

#include <memory>

#include "gui/context.h"
#include "utility/prop_info.h"

namespace yaga {

class PropertyViewerImpl;
typedef std::shared_ptr<PropertyViewerImpl> PropertyViewerImplPtr;

class PropertyViewer
{
public:
  explicit PropertyViewer(GuiContext context);
  void target(PropInfo* value);
  void docking(Docking value);

private:
  PropertyViewerImplPtr impl_;
};

typedef std::unique_ptr<PropertyViewer> PropertyViewerPtr;

} // !namespace yaga

#endif // !YAGA_GUI_PROPERTY_VIEWER
