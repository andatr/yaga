#include "precompiled.h"
#include "demo_application.h"

namespace yaga {
namespace demo {

// -----------------------------------------------------------------------------------------------------------------------------
DemoApplication::DemoApplication(assets::Serializer* serializer) :
  running_(false),
  serializer_(serializer),
  assets_(std::make_unique<assets::Storage>()),
  context_(nullptr),
  input_(nullptr),
  camera_(nullptr),
  cameraTransform_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
DemoApplication::~DemoApplication()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void DemoApplication::init(Context* context, Input* input)
{
  running_ = true;
  context_ = context;
  input_ = input;
  createCamera();
}

// -----------------------------------------------------------------------------------------------------------------------------
void DemoApplication::createCamera()
{
  cameraTransform_    = addAsset<assets::Transform>("camera_transform");
  auto cameraAsset    = addAsset<assets::Camera>("camera");
  auto object         = addObject<Object>("camera");
  camera_             = object->addComponent(context_->createCamera(cameraAsset));
  auto transform      = object->addComponent(context_->createTransform(cameraTransform_));
  context_->mainCamera(camera_);
  cameraAsset->lookAt({ 0.0f, 0.0f, 0.0f });
  cameraTransform_->translation({ 0.0f, 3.0f, 10.0f });
  resize();
}

// -----------------------------------------------------------------------------------------------------------------------------
void DemoApplication::resize()
{
  const auto& res = context_->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 100.0f);
  projection[1][1] *= -1;
  camera_->projection(projection);
}

// -----------------------------------------------------------------------------------------------------------------------------
bool DemoApplication::loop()
{
  return running_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void DemoApplication::stop()
{
  running_ = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
void DemoApplication::shutdown()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void DemoApplication::gui()
{
}

} // !namespace demo
} // !namespace yaga
