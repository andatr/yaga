#include "precompiled.h"
#include "model_viewer.h"
#include "assets/scene.h"
#include "engine/application.h"

namespace yaga {

// -------------------------------------------------------------------------------------------------------------------------
GamePtr createGame(assets::SerializerPtr serializer, assets::StoragePtr storage)
{
  return std::make_unique<ModelViewer>(std::move(serializer), std::move(storage));
}

// -------------------------------------------------------------------------------------------------------------------------
ModelViewer::ModelViewer(assets::SerializerPtr serializer, assets::StoragePtr storage) :
  BasicGame(std::move(serializer), std::move(storage))
{
}

// -------------------------------------------------------------------------------------------------------------------------
ModelViewer::~ModelViewer()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void ModelViewer::init(Application* app)
{
  base::init(app);

  auto context = app_->renderingContext();
  auto asset = serializer_->deserialize<assets::Scene>("scene", persistentAssets_.get())->model();

  auto object = std::make_unique<Object>();
  auto object_ptr = object.get();
  objects_.emplace_back(std::move(object));
  auto transform = std::make_unique<Transform>(object_ptr);
  objectPosition_ = transform.get();
  object_ptr->addComponent(std::move(transform));
  object_ptr->addComponent(context->createMesh(object_ptr, asset->mesh()));
  object_ptr->addComponent(context->createMaterial(object_ptr, asset->material()));
  object_ptr->addComponent(context->createRenderer3D(object_ptr));

  object = std::make_unique<Object>();
  object_ptr = object.get();
  objects_.emplace_back(std::move(object));
  transform = std::make_unique<Transform>(object_ptr);

  cameraAsset_ = serializer_->deserialize<assets::Camera>("camera", persistentAssets_.get());
  cameraPosition_ = transform.get();
  object_ptr->addComponent(std::move(transform));
  auto camera = context->createCamera(object_ptr, cameraAsset_);
  camera_ = camera.get();
  object_ptr->addComponent(std::move(camera));

  const auto& res = context->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  cameraAsset_->projection(projection);
  cameraPosition_->local(glm::translate(glm::mat4(1.0f), { 6.0f, 6.0f, 6.0f }));
  cameraAsset_->lookAt(glm::vec3(0.0f, 1.0f, 0.0f));
}

// -------------------------------------------------------------------------------------------------------------------------
void ModelViewer::resize()
{
  auto res = app_->renderingContext()->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  cameraAsset_->projection(projection);
}

// -------------------------------------------------------------------------------------------------------------------------
void ModelViewer::loop(float delta)
{
  base::loop(delta);

  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  glm::vec3 translate(10.0f * sin(time), 3.0f + 3.0f * sin(time), 10.0f * cos(time));

  cameraPosition_->local(glm::translate(glm::mat4(1.0f), translate));

  auto context = app_->renderingContext();
  const auto& res = context->resolution();
  auto projection = glm::perspective(glm::radians(60.0f + 45.0f * sin(time)), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  auto cameraAsset = persistentAssets_->get<assets::Camera>("camera");
  cameraAsset->projection(projection);

  objectPosition_->local(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) * time, glm::vec3(0.0f, 0.0f, 1.0f)));
}

// -------------------------------------------------------------------------------------------------------------------------
void ModelViewer::shutdown()
{
  base::shutdown();
}

} // !namespace yaga
