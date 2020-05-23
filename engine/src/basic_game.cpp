#include "precompiled.h"
#include "basic_game.h"

#include "engine/application.h"
#include "engine/asset/scene.h"

namespace yaga
{

// -------------------------------------------------------------------------------------------------------------------------
BasicGame::BasicGame() :
  assets_(std::make_unique<asset::Database>())
{
}

// -------------------------------------------------------------------------------------------------------------------------
BasicGame::~BasicGame()
{
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::init()
{
  auto context = app_->renderingContext();
  auto asset = assets_->get<asset::Scene>("scene")->model();
  auto cameraAsset = assets_->get<asset::Camera>("camera");
  


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
  
  cameraPosition_ = transform.get();
  object_ptr->addComponent(std::move(transform));
  auto camera = context->createCamera(object_ptr, cameraAsset);
  camera_ = camera.get();
  object_ptr->addComponent(std::move(camera));

  const auto& res = context->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  cameraAsset->projection(projection);
  cameraPosition_->local(glm::translate(glm::mat4(1.0f), { 6.0f, 6.0f, 6.0f }));
  cameraAsset->lookAt(glm::vec3(0.0f, 1.0f, 0.0f));
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::loop(float delta)
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

  glm::vec3 translate(10.0f * sin(time), 3.0f + 3.0f * sin(time), 10.0f * cos(time));

  cameraPosition_->local(glm::translate(glm::mat4(1.0f), translate));

  auto context = app_->renderingContext();
  const auto& res = context->resolution();
  auto projection = glm::perspective(glm::radians(60.0f + 45.0f * sin(time)), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  auto cameraAsset = assets_->get<asset::Camera>("camera");
  cameraAsset->projection(projection);

  objectPosition_->local(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f) *time, glm::vec3(0.0f, 0.0f, 1.0f)));
}

// -------------------------------------------------------------------------------------------------------------------------
void BasicGame::shutdown()
{
}

} // !namespace yaga

