#ifndef YAGA_DEMO_SRC_DEMO_MODEL_DEMO
#define YAGA_DEMO_SRC_DEMO_MODEL_DEMO

#include <memory>

#include "demo_application.h"

namespace yaga {
namespace demo {

class ModelDemo : public DemoApplication
{
public:
  explicit ModelDemo(assets::Serializer* serializer);
  virtual ~ModelDemo();
  void init(Context* context, Input* input) override;
  void gui() override;

private:
  typedef DemoApplication base;
};

} // !namespace demo
} // !namespace yaga

#endif // !YAGA_DEMO_SRC_DEMO_MODEL_DEMO
