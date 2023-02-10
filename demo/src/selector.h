#ifndef YAGA_DEMO_SRC_SELECTOR
#define YAGA_DEMO_SRC_SELECTOR

#include <memory>
#include <vector>

#include "gui/context.h"

namespace yaga {
namespace demo {

class SelectorApp
{
public:
  virtual ~SelectorApp() {};
  virtual const std::vector<const char*>& demoList() = 0;
  virtual int  currentDemo()                         = 0;
  virtual void currentDemo(int value)                = 0;
  virtual void drawDemoGui()                         = 0; 
};

class SelectorImpl;
typedef std::shared_ptr<SelectorImpl> SelectorImplPtr;

class Selector
{
public:
  Selector(GuiContext context, SelectorApp* app);

private:
  SelectorImplPtr impl_;
};

typedef std::unique_ptr<Selector> SelectorPtr;

} // !namespace demo
} // !namespace yaga

#endif // !YAGA_DEMO_SRC_SELECTOR
