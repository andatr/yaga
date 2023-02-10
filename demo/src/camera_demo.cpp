#include "precompiled.h"
#include "camera_demo.h"
#include "widget.h"

namespace yaga {
namespace demo {
namespace {

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
CameraDemo::CameraDemo(assets::Serializer* serializer) :
  DemoApplication(serializer)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
CameraDemo::~CameraDemo()
{
}

// -----------------------------------------------------------------------------------------------------------------------------
void CameraDemo::init(Context* context, Input* input)
{
  base::init(context, input);
}

// -----------------------------------------------------------------------------------------------------------------------------
void CameraDemo::gui()
{
  base::gui();
}

} // !namespace demo
} // !namespace yaga
