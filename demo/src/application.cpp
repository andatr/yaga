#include "precompiled.h"
#include "application.h"
#include "camera_demo.h"
#include "model_demo.h"
#include "triline_demo.h"

namespace yaga {
namespace demo {
namespace {

typedef ApplicationPtr(*AppFactory)(assets::Serializer*);

const std::vector<const char*> DEMO_LIST = {
  "",
  "Triline",
  "Model",
  "Camera"
};

template <typename T>
ApplicationPtr createDemo(assets::Serializer*);
ApplicationPtr createEmptyDemo(assets::Serializer*);

const AppFactory DEMO_FACTORY[] = {
  &createEmptyDemo,
  &createDemo<TrilineDemo>,
  &createDemo<ModelDemo>,
  &createDemo<CameraDemo>
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
ApplicationPtr createDemo(assets::Serializer* serializer)
{
  return std::make_unique<T>(serializer);
}

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr createEmptyDemo(assets::Serializer*)
{
  return nullptr;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr createApplication(const boost::property_tree::ptree& options)
{
  return std::make_unique<Application>(options);
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::Application(const boost::property_tree::ptree& options) :
  BasicApplication(options),
  demoIndex_(0),
  guiContext_(this, nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::init(Context* context, Input* input)
{
  base::init(context, input);
  selector_ = std::make_unique<Selector>(guiContext_, this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::resize()
{
  base::resize();
  if (demo_) {
    demo_->resize();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::loop()
{
  base::loop();
  if (demo_) {
    demo_->loop();
  }
  return running_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::gui()
{
  base::gui();
  dispatcher_.process();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::shutdown()
{
  if (demo_) {
    demo_->shutdown();
    demo_ = nullptr;
  }
  taskPool_.join();
  dispatcher_.reset();
  selector_ = nullptr;
  base::shutdown();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::stop()
{
  if (demo_) {
    demo_->stop();
  }
  base::stop();
}

// -----------------------------------------------------------------------------------------------------------------------------
ImVec2 Application::screenSize()
{
  return context_->resolution();
}

// -----------------------------------------------------------------------------------------------------------------------------
Dispatcher& Application::dispatcher()
{
  return dispatcher_;
}

// -----------------------------------------------------------------------------------------------------------------------------
boost::asio::thread_pool& Application::taskPool()
{
  return taskPool_;
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::vector<const char*>& Application::demoList()
{
  return DEMO_LIST;
}

// -----------------------------------------------------------------------------------------------------------------------------
int Application::currentDemo()
{
  return demoIndex_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::currentDemo(int value)
{
  demoIndex_ = value;
  if (demo_) {
    demo_->shutdown();
  }
  demo_ = DEMO_FACTORY[demoIndex_](serializer_.get());
  if (demo_) {
    demo_->init(context_, input_);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::drawDemoGui()
{
  if (demo_) {
    demo_->gui();
  }
}

} // !namespace demo
} // !namespace yaga
