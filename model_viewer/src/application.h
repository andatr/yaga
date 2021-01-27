#ifndef YAGA_MODEL_VIEWER_SRC_APPLICATION
#define YAGA_MODEL_VIEWER_SRC_APPLICATION

#include <string>
#include <memory>
#include <boost/asio.hpp>

#include "assets/mesh.h"
#include "engine/basic_application.h"
#include "engine/camera.h"
#include "engine/object.h"

namespace yaga {
namespace mview {

struct MeshMetadata;
class Gui;
typedef std::unique_ptr<Gui> GuiPtr;

class Application : public BasicApplication
{
friend class Gui;

public:
  Application(const boost::property_tree::ptree& options);
  virtual ~Application();
  void init(Context* context, Input* input) override;
  void resize()   override;
  bool loop()     override;
  void stop()     override;
  void shutdown() override;
  void gui()      override;

private:
  typedef BasicApplication base;

private:
  assets::Mesh* importModel(const std::string& path, const std::string& path2, MeshMetadata& meta);
  void setupCamera();
  void createBoundingBox(assets::Mesh* modelMesh, const MeshMetadata& meta);
  void exit();
  void save(const std::string filename);
  void open(const std::string filename);

private:
  boost::asio::thread_pool taskPool_;
  GuiPtr gui_;
  std::vector<ObjectPtr> objects_;
  Camera* camera_;
  Transform* cameraPosition_;
  Transform* objectPosition_;
};

ApplicationPtr createApplication(const boost::property_tree::ptree& options);

} // !namespace mview
} // !namespace yaga

#endif // !YAGA_MODEL_VIEWER_SRC_APPLICATION
