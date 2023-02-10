#ifndef YAGA_DEMO_SRC_DEMO_CAMERA_DEMO
#define YAGA_DEMO_SRC_DEMO_CAMERA_DEMO

#include <memory>

#include "demo_application.h"

namespace yaga {
namespace demo {

class CameraDemo : public DemoApplication
{
public:
  explicit CameraDemo(assets::Serializer* serializer);
  virtual ~CameraDemo();
  void init(Context* context, Input* input) override;
  void gui() override;

private:
  typedef DemoApplication base;
};

} // !namespace demo
} // !namespace yaga

#endif // !YAGA_DEMO_SRC_DEMO_CAMERA_DEMO
