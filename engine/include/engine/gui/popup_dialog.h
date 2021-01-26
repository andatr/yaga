#ifndef YAGA_ENGINE_GUI_POPUP_DIALOG
#define YAGA_ENGINE_GUI_POPUP_DIALOG

#include <functional>
#include <string>
#include <memory>

#include "dialog.h"
#include "engine/gui/gui_context.h"

namespace yaga {

typedef std::function<void(DialogResult)> PopupDialogHandle;

void renderPopupDialog(GuiContext context, const std::string& id, const std::string& content,
  DialogResult buttons, PopupDialogHandle handle = [](DialogResult) {});

void renderPopupDialog(GuiContext context, const std::string& id, const std::string& content,
  std::shared_ptr<bool> show);

} // !namespace yaga

#endif // !YAGA_ENGINE_GUI_POPUP_DIALOG
