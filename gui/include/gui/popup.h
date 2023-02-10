#ifndef YAGA_GUI_POPUP_DIALOG
#define YAGA_GUI_POPUP_DIALOG

#include <functional>
#include <string>

#include "gui/context.h"

namespace yaga {
namespace popup {

enum class Result
{
  None             = 0,
  Yes              = 1,
  No               = 2,
  Retry            = 4,
  Cancel           = 8,
  YesNo            = 3,
  YesNoCancel      = 11,
  YesNoRetry       = 7,
  YesNoRetryCancel = 15
};

typedef std::function<void(Result)> Handle;
typedef std::function<bool(void)> ShowHandle;

void render(
  GuiContext         context,
  const std::string& id,
  const std::string& content,
  Result             buttons,
  Handle             handle = [](Result) {}
);

void render(
  GuiContext         context,
  const std::string& id,
  const std::string& content,
  ShowHandle         show
);

} // !namespace popup
} // !namespace yaga

#endif // !YAGA_GUI_POPUP_DIALOG
