#ifndef YAGA_DEMO_SRC_APPLICATION
#define YAGA_DEMO_SRC_APPLICATION

#include <memory>

#include "selector.h"
#include "engine/basic_application.h"
#include "gui/dispatcher.h"
#include "gui/context.h"

namespace yaga {
namespace demo {

class Application  :
  public BasicApplication,
  public GuiContextApp,  
  public SelectorApp
{
public:
  Application(const boost::property_tree::ptree& options);
  virtual ~Application();
  // BasicApplication
  void init(Context* context, Input* input) override;
  void resize()   override;
  bool loop()     override;
  void stop()     override;
  void shutdown() override;
  void gui()      override;
  // GuiContextApp
  ImVec2 screenSize() override;
  Dispatcher& dispatcher() override;
  boost::asio::thread_pool& taskPool() override;
  // SelectorApp
  const std::vector<const char*>& demoList() override;
  int currentDemo() override;
  void currentDemo(int value) override;
  void drawDemoGui() override;

private:
  typedef BasicApplication base;

private:
  int demoIndex_;
  boost::asio::thread_pool taskPool_;
  GuiContext guiContext_;
  Dispatcher dispatcher_;
  SelectorPtr selector_;
  ApplicationPtr demo_;
};

ApplicationPtr createApplication(const boost::property_tree::ptree& options);

} // !namespace demo
} // !namespace yaga

#endif // !YAGA_DEMO_SRC_APPLICATION
