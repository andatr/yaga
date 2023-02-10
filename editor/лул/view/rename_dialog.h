#ifndef YAGA_EDITOR_SRC_VIEW_RENAME_DIALOG
#define YAGA_EDITOR_SRC_VIEW_RENAME_DIALOG

#include <functional>
#include <string>

#include "gui/context.h"

namespace yaga {
namespace rename {

typedef std::function<void(std::string)> Handle;

void render(GuiContext context, const std::string& value, Handle handle);

} // !namespace rename
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_VIEW_RENAME_DIALOG
