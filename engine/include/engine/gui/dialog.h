#ifndef YAGA_ENGINE_GUI_DIALOG
#define YAGA_ENGINE_GUI_DIALOG

namespace yaga {

enum class DialogResult
{
  None = 0,
  Yes = 1,
  No = 2,
  Retry = 4,
  Cancel = 8,
  YesNo = 3,
  YesNoCancel = 11,
  YesNoRetry = 7,
  YesNoRetryCancel = 15
};

} // !namespace yaga

#endif // !YAGA_ENGINE_GUI_DIALOG
