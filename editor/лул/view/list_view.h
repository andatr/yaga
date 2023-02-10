#ifndef YAGA_EDITOR_SRC_VIEW_LIST_VIEW
#define YAGA_EDITOR_SRC_VIEW_LIST_VIEW

#include <memory>
#include <string>
#include <vector>

#include "list_view_data.h"
#include "gui/property_viewer.h"

namespace yaga {
namespace editor {

class ListViewImpl;

class ListView
{
public:
  ListView(GuiContext context, const std::string& name, ListViewData* data);

private:
  std::shared_ptr<ListViewImpl> impl_;
};

typedef std::unique_ptr<ListView> ListViewPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_VIEW_LIST_VIEW
