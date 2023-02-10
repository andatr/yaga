#include "precompiled.h"
#include "model_demo.h"
#include "widget.h"

namespace yaga {
namespace demo {
namespace {

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
ModelDemo::ModelDemo(assets::Serializer* serializer) :
  DemoApplication(serializer)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ModelDemo::~ModelDemo()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelDemo::init(Context* context, Input* input)
{
  base::init(context, input);
  /*auto material  = serializer_->deserialize<assets::Material>("triline_material", assets_.get());
  auto meshPtr   = triline::createMesh("triline_mesh", points_.data(), points_.size(), trilineProps_, edgeType());
  mesh_          = meshPtr.get();
  assets_->put(std::move(meshPtr));
  transform_     = addAsset<assets::Transform>("triline_transform");
  auto object    = addObject<Object>("triline");
  object->addComponent(context_->createMesh(mesh_));
  object->addComponent(context_->createTransform(transform_));
  material_ = object->addComponent(context_->createMaterial(material));
  object->addComponent(context_->createRenderer3D());*/
}

// -----------------------------------------------------------------------------------------------------------------------------
void ModelDemo::gui()
{
  base::gui();
}

} // !namespace demo
} // !namespace yaga
