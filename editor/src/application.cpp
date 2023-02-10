#include "precompiled.h"
#include "application.h"

namespace yaga {
namespace editor {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
unsigned int getTaskPoolSize()
{
  auto size = std::thread::hardware_concurrency();
  if (size > 1) return size - 1;
  return 1;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
Application::Application(ConfigPtr config, const std::string& projectPath) :
  BasicApplication(config),
  taskPool_(getTaskPoolSize()),
  docking_(this),
  guiContext_(this, &docking_),
  hasChanges_(false),
  filename_(projectPath)
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
  //meshList_ = std::make_unique<MeshList>(context_, serializer_.get(), this->assets_.get());
  //menu_ = std::make_unique<MainMenu>(guiContext_, this);
  //file_dialog::setDefaultDir(workingDir_);
  //assetManager_   = std::make_unique<AssetManager>(workingDir_);
 // propertyViewer_ = std::make_unique<PropertyViewer>(guiContext_);
  //propertyViewer_->docking(Docking::Right);
  //assetView_ = std::make_unique<AssetView>(guiContext_, nullptr);

  //meshListView_ = std::make_unique<ListView>(guiContext_, "MeshList", meshList_.get());
  //connections_.push_back(meshList_->properties(MeshList::PropertyIndex::selected)->onUpdate([this](void*) {
  //  propertyViewer_->target(meshList_->selectedEntry());
  //}));
  assetManager_ = std::make_unique<AssetManager>();
  gui_ = std::make_unique<Gui>(guiContext_, this);
  if (!filename_.empty()) open(filename_);
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::resize()
{
  base::resize();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::loop()
{
  return running_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::gui()
{
  docking_.update();
  dispatcher_.process();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::shutdown()
{
  taskPool_.join();
  dispatcher_.reset();
  gui_ = nullptr;
  assetManager_ = nullptr;
  base::shutdown();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::stop()
{
  gui_->exit();
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
const std::string& Application::filename() const
{
  return filename_;
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetManager* Application::assetManager()
{ 
  return assetManager_.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::hasChanges() const
{
  return hasChanges_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::hasChanges(bool value)
{
  hasChanges_ = value;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::save(const std::string& filename, Func next)
{
  hasChanges_ = false;
  filename_ = filename;
  gui_->showPopup("Saving...");
  guiContext_.postCon(
    [this]() { assetManager_->save(); },
    [this, next]() { gui_->hidePopup(); next(); }
  );
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::open(const std::string& filename)
{
  hasChanges_ = false;
  filename_ = filename;
  gui_->showPopup("Loading...");
  guiContext_.postCon(
    [this]() { assetManager_->load(filename_); },
    [this]() { gui_->hidePopup(); }
  );
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::exit()
{
  running_ = false;
}

} // !namespace editor
} // !namespace yaga
