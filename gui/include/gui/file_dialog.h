#ifndef YAGA_GUI_FILE_DIALOG
#define YAGA_GUI_FILE_DIALOG

#include <functional>
#include <string>

#include "gui/context.h"

namespace yaga {
namespace file_dialog {

enum class Mode
{
  Open,
  Save
};

typedef std::function<void(std::string)> Handle;

void render(
  GuiContext         context,
  Mode               mode,
  const std::string& path,
  Handle             handle = [](const std::string&) {}
);

void render(
  GuiContext context,
  Mode       mode,
  Handle     handle = [](const std::string&) {}
);

void setDefaultDir(const std::string& path);

} // !namespace file_dialog
} // !namespace yaga

#endif // !YAGA_GUI_FILE_DIALOG
