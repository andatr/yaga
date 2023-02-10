#ifndef YAGA_DEMO_SRC_DEMO_TRILINE_DEMO
#define YAGA_DEMO_SRC_DEMO_TRILINE_DEMO

#include <memory>

#include "demo_application.h"
#include "engine/tools/triline.h"

namespace yaga {
namespace demo {

class TrilineDemo : public DemoApplication
{
public:
  explicit TrilineDemo(assets::Serializer* serializer);
  virtual ~TrilineDemo();
  void init(Context* context, Input* input) override;
  void gui() override;

private:
  void updateMesh(bool recreate);

private:
  typedef DemoApplication base;

private:
  triline::EdgeType edgeType() const;

private:
  std::array<glm::vec3, 3> points_;
  bool wireframe_;
  triline::VertexProps trilineProps_;
  int edge_;
  float angle_;
  float rotation_;
  assets::Mesh* mesh_;
  assets::Transform* transform_;
  Material* material_;
};

} // !namespace demo
} // !namespace yaga

#endif // !YAGA_DEMO_SRC_DEMO_TRILINE_DEMO
