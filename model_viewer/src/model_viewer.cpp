#include "precompiled.h"
#include "model_viewer.h"
#include "boundingBox.h"
#include "importer.h"
#include "assets/scene.h"
#include "engine/debug.h"
#include "engine/platform.h"

namespace yaga {
namespace {

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
ApplicationPtr createApplication(assets::Serializer* serializer)
{
  return std::make_unique<ModelViewer>(serializer);
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelViewer::ModelViewer(assets::Serializer* serializer) :
  BasicApplication(serializer)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelViewer::~ModelViewer()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelViewer::init(RenderingContext* renderer, Input* input)
{
  base::init(renderer, input);

  MeshMetadata meta {};
  auto m = importModel(R"(C:\Projects\Cpp\VulkanGame2\data\models\37-rp_eric_rigged_001_fbx\rp_eric_rigged_001_yup_a.fbx)", "material", meta);
  setupCamera();
  createBoundingBox(m, meta);

  auto m2 = importModel(R"(C:\Projects\Cpp\VulkanGame2\data\models\37-rp_eric_rigged_001_fbx\rp_eric_rigged_001_zup_a.fbx)", "material2", meta);
  objectPosition_->local(glm::translate(objectPosition_->local(), { 0.0f, 0.0f, -100.0f }));
}

// -----------------------------------------------------------------------------------------------------------------------------
 assets::Mesh* ModelViewer::importModel(const std::string& path, const std::string& mname, MeshMetadata& meta)
{
  auto objectUnique = std::make_unique<Object>();
  auto object = objectUnique.get();
  objects_.emplace_back(std::move(objectUnique));

  auto mesh = importMesh(path, meta);
  auto m = mesh.get();
  persistentAssets_->put(std::move(mesh));

  object->addComponent(renderer_->createMesh(object, m));
  glm::vec3 bounds = meta.max - meta.min;
  auto max = 10.0f / std::max(std::max(bounds.x, bounds.y), bounds.z);
  glm::vec3 mid = (meta.max + meta.min) / 2.0f;

  auto transform = std::make_unique<Transform>(object);
  objectPosition_ = transform.get();

  objectPosition_->local(glm::translate(glm::scale(objectPosition_->local(), { max, max, max }), -mid));
  object->addComponent(std::move(transform));

  auto material = serializer_->deserialize<assets::Material>(mname, persistentAssets_.get());
  object->addComponent(renderer_->createMaterial(object, material));
  object->addComponent(renderer_->createRenderer3D(object));

  return m;
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelViewer::createBoundingBox(assets::Mesh* modelMesh, const MeshMetadata& meta)
{
  auto objectUnique = std::make_unique<Object>();
  auto object = objectUnique.get();
  objects_.emplace_back(std::move(objectUnique));

  auto mesh = makeBoundingBox(modelMesh, meta);
  object->addComponent(renderer_->createMesh(object, mesh.get()));
 
  glm::vec3 bounds = meta.max - meta.min;
  float max =  10.0f / std::max(std::max(bounds.x, bounds.y), bounds.z);
  glm::vec3 mid = (meta.max + meta.min) / 2.0f;

  auto transform = std::make_unique<Transform>(object);
  auto g = transform.get();

  g->local(glm::translate(glm::scale(g->local(), { max, max, max }), -mid));
  object->addComponent(std::move(transform));

  auto material = serializer_->deserialize<assets::Material>("ui_3d_mat", persistentAssets_.get());
  object->addComponent(renderer_->createMaterial(object, material));
  object->addComponent(renderer_->createRenderer3D(object));
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelViewer::setupCamera()
{
  auto objectUnique = std::make_unique<Object>();
  auto object = objectUnique.get();
  objects_.emplace_back(std::move(objectUnique));
  
  auto transform = std::make_unique<Transform>(object);
  cameraPosition_ = transform.get();
  object->addComponent(std::move(transform));

  cameraAsset_ = serializer_->deserialize<assets::Camera>("camera", persistentAssets_.get());
  auto camera = renderer_->createCamera(object, cameraAsset_);
  camera_ = camera.get();
  object->addComponent(std::move(camera));

  const auto& res = renderer_->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  cameraAsset_->projection(projection);
  cameraPosition_->local(glm::translate(glm::mat4(5.0f), { 0.0f, 0.0f, 6.0f }));
  cameraAsset_->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelViewer::resize()
{
  auto res = renderer_->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  cameraAsset_->projection(projection);
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelViewer::loop(float delta)
{
  base::loop(delta);

  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  glm::vec3 translate(10.0f * sin(0.1f * time), 3.0f + 3.0f * sin(0.1f * time), 10.0f * cos(0.1f * time));
  cameraPosition_->local(glm::translate(glm::mat4(1.0f), translate));
  
  /*auto context = app_->renderingContext();
  const auto& res = context->resolution();
  auto projection = glm::perspective(glm::radians(60.0f + 45.0f * sin(time)), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  auto cameraAsset = persistentAssets_->get<assets::Camera>("camera");
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
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelViewer::shutdown()
{
  base::shutdown();
}

} // !namespace yaga
