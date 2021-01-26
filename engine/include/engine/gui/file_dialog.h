#ifndef YAGA_ENGINE_GUI_FILE_DIALOG
#define YAGA_ENGINE_GUI_FILE_DIALOG

#include <functional>
#include <string>

#include "engine/gui/gui_context.h"

namespace yaga {

enum class FileDialogMode
{
  Open,
  Save
};

typedef std::function<void(std::string)> FileDialogHandle;

void renderFileDialog(GuiContext context, FileDialogMode mode, const std::string& path,
  FileDialogHandle handle = [](const std::string&) {});

} // !namespace yaga

#endif // !YAGA_ENGINE_GUI_FILE_DIALOG
