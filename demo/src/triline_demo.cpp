#include "precompiled.h"
#include "triline_demo.h"
#include "widget.h"

namespace yaga {
namespace demo {
namespace {

constexpr float TRILINE_WIDTH = 2.0f;

const std::vector<const char*> EDGE_TYPE = {
  "Blunt",
  "Cap",
  "Cycle"
};

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
TrilineDemo::TrilineDemo(assets::Serializer* serializer) :
  DemoApplication(serializer),
  wireframe_(false),
  edge_(0),
  angle_(180.0f),
  rotation_(0.0f),
  mesh_(nullptr),
  transform_(nullptr),
  material_(nullptr)
{
  points_ = { 
    glm::vec3(-TRILINE_WIDTH, 0.0f, 0.0f),
    glm::vec3(          0.0f, 0.0f, 0.0f),
    glm::vec3( TRILINE_WIDTH, 0.0f, 0.0f)
  };
  trilineProps_.thickness = 1.0f;
  trilineProps_.feathering = 0.2f;
  trilineProps_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
}

// -----------------------------------------------------------------------------------------------------------------------------
TrilineDemo::~TrilineDemo()
{
}

// ----------------------------------------------------------------------------------------------------------------------------
triline::EdgeType TrilineDemo::edgeType() const
{
  switch(edge_) {
  case 0:
    return triline::EdgeType::Blunt;
  case 1:
    return triline::EdgeType::Cap;
  case 2:
    return triline::EdgeType::Cycle;
  default:
    return triline::EdgeType::Blunt;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void TrilineDemo::init(Context* context, Input* input)
{
  base::init(context, input);
  auto material  = serializer_->deserialize<assets::Material>("triline_material", assets_.get());
  auto meshPtr   = triline::createMesh("triline_mesh", points_.data(), points_.size(), trilineProps_, edgeType());
  mesh_          = meshPtr.get();
  assets_->put(std::move(meshPtr));
  transform_     = addAsset<assets::Transform>("triline_transform");
  auto object    = addObject<Object>("triline");
  object->addComponent(context_->createMesh(mesh_));
  object->addComponent(context_->createTransform(transform_));
  material_ = object->addComponent(context_->createMaterial(material));
  object->addComponent(context_->createRenderer3D());
}

// -----------------------------------------------------------------------------------------------------------------------------
void TrilineDemo::updateMesh(bool recreate)
{
  if (recreate) {
    auto vCount = triline::getVertexCount(points_.size(), edgeType());
    auto iCount = triline::getIndexCount (points_.size(), edgeType());
    mesh_->vertices([this, vCount](auto& vertices) {
      vertices.resize(vCount);
      triline::updateVertices(points_.data(), points_.size(), vertices.data(), trilineProps_, edgeType());
    });
    mesh_->indices([this, iCount](auto& indices) {
      indices.resize(iCount);
      triline::updateIndices(points_.data(), points_.size(), indices.data(), edgeType());
    });
  }
  else {
    mesh_->vertices([this](auto& vertices) {
      triline::updateVertices(points_.data(), points_.size(), vertices.data(), trilineProps_, edgeType());
    });
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void TrilineDemo::gui()
{
  base::gui();
  auto oldEdge = edge_;
  bool update = false;
  if (ImGui::Checkbox("Wireframe", &wireframe_)) {
    material_->wireframe(wireframe_);
  }
  ImGui::TextUnformatted("Rotation");
  ImGui::SetNextItemWidth(WIDGET_WIDTH);
  if (ImGui::DragFloat("##Triline#Rotation", &rotation_, 1.0f, 0.0f, 360.0f)) {
    transform_->rotation(glm::quat_cast(glm::orientate4(glm::vec3(0.0f, 0.0f, rotation_ * glm::pi<float>() / 180.0f))));
  }
  ImGui::TextUnformatted("Edge Type");
  ImGui::SetNextItemWidth(WIDGET_WIDTH);
  update |= ImGui::Combo("##Triline#Edge", &edge_, EDGE_TYPE.data(), static_cast<int>(EDGE_TYPE.size()));
  ImGui::TextUnformatted("Angle");
  ImGui::SetNextItemWidth(WIDGET_WIDTH);
  if (ImGui::DragFloat("##Triline#Angle", &angle_, 1.0f, 90.0f, 180.0f)) {
    update = true;
    const auto angle = (180.0f - angle_) * glm::pi<float>() / 180.0f;
    points_[2].x = TRILINE_WIDTH * cos(angle);
    points_[2].y = TRILINE_WIDTH * sin(angle);
  }
  ImGui::TextUnformatted("Thickness");
  ImGui::SetNextItemWidth(WIDGET_WIDTH);
  update |= ImGui::DragFloat("##Triline#Thickness", &trilineProps_.thickness, 0.1f, 0.1f, 2.0f);
  ImGui::TextUnformatted("Feathering");
  ImGui::SetNextItemWidth(WIDGET_WIDTH);
  update |= ImGui::DragFloat("##Triline#Feathering", &trilineProps_.feathering, 0.1f, 0.1f, 1.0f);
  ImGui::TextUnformatted("Color");
  ImGui::SetNextItemWidth(WIDGET_WIDTH);
  constexpr auto flag = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview;
  update |= ImGui::ColorPicker4("##Triline#Color", (float*)&trilineProps_.color, flag);
  if (update) {
    updateMesh(edge_ != oldEdge);
  }
}

} // !namespace demo
} // !namespace yaga
