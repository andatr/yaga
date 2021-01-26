#include "precompiled.h"
#include "application.h"
#include "boundingBox.h"
#include "gui.h"
#include "importer.h"
#include "assets/camera.h"
#include "assets/scene.h"
#include "engine/debug.h"
#include "engine/platform.h"

namespace yaga {
namespace mview {
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
ApplicationPtr createApplication(const boost::property_tree::ptree& options)
{
  return std::make_unique<Application>(options);
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::Application(const boost::property_tree::ptree& options) :
  BasicApplication(options),
  running_(false),
  taskPool_(getTaskPoolSize())
{
  gui_ = std::make_unique<Gui>(options, this);
}

// -----------------------------------------------------------------------------------------------------------------------------
Application::~Application()
{
  taskPool_.join();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::init(Context* context, Input* input)
{
  base::init(context, input);
  running_ = true;

  MeshMetadata meta {};
  auto m = importModel(R"(C:\Projects\Cpp\VulkanGame2\data\models\37-rp_eric_rigged_001_fbx\rp_eric_rigged_001_yup_a.fbx)", "material", meta);
  setupCamera();
  createBoundingBox(m, meta);

  auto m2 = importModel(R"(C:\Projects\Cpp\VulkanGame2\data\models\37-rp_eric_rigged_001_fbx\rp_eric_rigged_001_zup_a.fbx)", "material2", meta);
  objectPosition_->local(glm::translate(objectPosition_->local(), { 0.0f, 0.0f, -100.0f }));
}

// -----------------------------------------------------------------------------------------------------------------------------
 assets::Mesh* Application::importModel(const std::string& path, const std::string& mname, MeshMetadata& meta)
{
  auto objectUnique = std::make_unique<Object>();
  auto object = objectUnique.get();
  objects_.emplace_back(std::move(objectUnique));

  auto mesh = importMesh(path, meta);
  auto m = mesh.get();
  assets_->put(std::move(mesh));

  object->addComponent(context_->createMesh(object, m));
  glm::vec3 bounds = meta.max - meta.min;
  auto max = 10.0f / std::max(std::max(bounds.x, bounds.y), bounds.z);
  glm::vec3 mid = (meta.max + meta.min) / 2.0f;

  auto transform = std::make_unique<Transform>(object);
  objectPosition_ = transform.get();

  objectPosition_->local(glm::translate(glm::scale(objectPosition_->local(), { max, max, max }), -mid));
  object->addComponent(std::move(transform));

  auto material = serializer_->deserialize<assets::Material>(mname, assets_.get());
  auto mm = context_->createMaterial(object, material);
  mm->wireframe(true);
  object->addComponent(std::move(mm));
  object->addComponent(context_->createRenderer3D(object));

  return m;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::createBoundingBox(assets::Mesh* modelMesh, const MeshMetadata& meta)
{
  auto objectUnique = std::make_unique<Object>();
  auto object = objectUnique.get();
  objects_.emplace_back(std::move(objectUnique));

  auto mesh = makeBoundingBox(modelMesh, meta);
  object->addComponent(context_->createMesh(object, mesh.get()));
 
  glm::vec3 bounds = meta.max - meta.min;
  float max =  10.0f / std::max(std::max(bounds.x, bounds.y), bounds.z);
  glm::vec3 mid = (meta.max + meta.min) / 2.0f;

  auto transform = std::make_unique<Transform>(object);
  auto g = transform.get();

  g->local(glm::translate(glm::scale(g->local(), { max, max, max }), -mid));
  object->addComponent(std::move(transform));

  auto material = serializer_->deserialize<assets::Material>("ui_3d_mat", assets_.get());
  object->addComponent(context_->createMaterial(object, material));
  object->addComponent(context_->createRenderer3D(object));
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::setupCamera()
{
  auto objectUnique = std::make_unique<Object>();
  auto object = objectUnique.get();
  objects_.emplace_back(std::move(objectUnique));
  
  auto transform = std::make_unique<Transform>(object);
  cameraPosition_ = transform.get();
  object->addComponent(std::move(transform));

  //auto cameraAsset = serializer_->deserialize<assets::Camera>("camera", assets_.get());
  auto camera = context_->createCamera(object);
  camera_ = camera.get();
  object->addComponent(std::move(camera));
  context_->mainCamera(camera_);

  const auto& res = context_->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  camera_->projection(projection);
  cameraPosition_->local(glm::translate(glm::mat4(5.0f), { 0.0f, 0.0f, 6.0f }));
  camera_->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::resize()
{
  gui_->resize();
  auto res = context_->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  camera_->projection(projection);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Application::loop()
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  glm::vec3 translate(10.0f * sin(0.1f * time), 3.0f + 3.0f * sin(0.1f * time), 10.0f * cos(0.1f * time));
  cameraPosition_->local(glm::translate(glm::mat4(1.0f), translate));
  
  /*auto context = app_->renderingContext();
  const auto& res = context->resolution();
  auto projection = glm::perspective(glm::radians(60.0f + 45.0f * sin(time)), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  auto cameraAsset = assets_->get<assets::Camera>("camera");
  cameraAsset->projection(projection);*/

  //

  /*
  auto pos = objectPosition_->local();
  auto input = input_->getState();
  if (input.keys[keys::w].pressed) {
    pos = glm::translate(pos, glm::vec3(0.001f * delta, 0.0f, 0.0f));
  }
  if (input.keys[keys::s].pressed) {
    pos = glm::translate(pos, glm::vec3(-0.001f * delta, 0.0f, 0.0f));
  }
  if (input.keys[keys::a].pressed) {
    pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, 0.001f * delta));
  }
  if (input.keys[keys::d].pressed) {
    pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, -0.001f * delta));
  }
  if (input.keys[keys::home].pressed) {
    pos = glm::mat4(1.0f);
  }
  objectPosition_->local(pos);*/
  
  return running_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::gui()
{
  gui_->render();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::shutdown()
{
  base::shutdown();
  objects_.clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::save(const std::string filename)
{
  std::this_thread::sleep_for(std::chrono::seconds(10));
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::open(const std::string filename)
{
  std::this_thread::sleep_for(std::chrono::seconds(10));
}

// -----------------------------------------------------------------------------------------------------------------------------
void Application::exit()
{
  running_ = false;
}

} // !namespace mview
} // !namespace yaga
