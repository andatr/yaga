#include "precompiled.h"
#include "gui/property_editor_registry.h"
#include "gui/property_editor_standard.h"
#include "utility/prop_info.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
PropertyEditorCategory::PropertyEditorCategory(IProperty* prop) :
  PropertyEditorCategory(static_cast<PropInfo*>(prop->get()))
{
  prop_ = prop;
  root_ = false;
}

// -----------------------------------------------------------------------------------------------------------------------------
PropertyEditorCategory::PropertyEditorCategory(PropInfo* propInfo) :
  prop_(nullptr),
  root_(true)
{
  auto& factory = PropertyEditorRegistry::instance();
  for (auto& p : propInfo->properties()) {
    if (auto editor = factory.createEditor(p.get())) {
      if (factory.hidden(typeid(*propInfo), p->name())) continue;
      editors_.push_back(std::move(editor));
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void PropertyEditorCategory::render()
{
  if (root_ || ImGui::CollapsingHeader(prop_->name().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
    for (auto& editor : editors_) {
      editor->render();
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
PropertyEditorBase::PropertyEditorBase(IProperty* prop) :
  prop_(prop),
  flags_(ImGuiInputTextFlags_EnterReturnsTrue)
{
  if (prop_->readOnly()) {
    flags_ |= ImGuiInputTextFlags_ReadOnly;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
StringPropertyEditor::StringPropertyEditor(IProperty* prop) :
  PropertyEditorBase(prop)
{
  buffer_.resize(512);
  auto value = static_cast<std::string*>(prop_->get());
  std::memcpy(buffer_.data(), value->c_str(), std::min((size_t)512, value->size()));
  name_ = "##StringPropertyEditor" + prop_->name();
}

// -----------------------------------------------------------------------------------------------------------------------------
void StringPropertyEditor::render()
{
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(prop_->name().c_str());
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
  ImGui::InputText(name_.c_str(), buffer_.data(), buffer_.size(), flags_);
  ImGui::PopItemWidth();
}

// -----------------------------------------------------------------------------------------------------------------------------
IntPropertyEditor::IntPropertyEditor(IProperty* prop) :
  PropertyEditorBase(prop)
{
  name_ = "##IntPropertyEditor" + prop_->name();
}

// -----------------------------------------------------------------------------------------------------------------------------
void IntPropertyEditor::render()
{
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(prop_->name().c_str());
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
  ImGui::InputScalar(name_.c_str(), ImGuiDataType_S64, prop_->get(), 0, 0, nullptr, flags_);
  ImGui::PopItemWidth();
}

// -----------------------------------------------------------------------------------------------------------------------------
Vec3PropertyEditor::Vec3PropertyEditor(IProperty* prop) :
  PropertyEditorBase(prop)
{
  label_[0]     = "X";
  label_[1]     = "Y";
  label_[2]     = "Z";
  inputName_[0] = "##X#Vec3PropertyEditor" + prop_->name();
  inputName_[1] = "##Y#Vec3PropertyEditor" + prop_->name();
  inputName_[2] = "##Z#Vec3PropertyEditor" + prop_->name();
  sizes_[0] = sizes_[1] = sizes_[2] = ImGui::GetFontSize();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool Vec3PropertyEditor::renderInput(size_t index, float& value, float width)
{
  bool result = false;
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(label_[index].c_str());
  sizes_[index] = ImGui::GetItemRectSize().x;
  ImGui::SameLine();
  ImGui::PushItemWidth(width);
  result = ImGui::InputFloat(inputName_[index].c_str(), &value, 0.0f, 0.0f, "%.2f", flags_);
  ImGui::PopItemWidth();
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
void Vec3PropertyEditor::render()
{
  const auto& style = ImGui::GetStyle();
  const float windowWidth = ImGui::GetContentRegionAvailWidth();
  const float width = (windowWidth - sizes_[0] - sizes_[1] - sizes_[2] - 5.0f * style.ItemSpacing.x) / 3.0f;
  auto value = static_cast<glm::vec3*>(prop_->get());
  ImGui::TextUnformatted(prop_->name().c_str());
  bool result = renderInput(0, value->x, width);
  ImGui::SameLine();
  result |= renderInput(1, value->y, width);
  ImGui::SameLine();
  result |= renderInput(2, value->z, width);
  if (result) {
    prop_->set(value, this);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
MeshBoundsPropertyEditor::MeshBoundsPropertyEditor(IProperty* prop) :
  PropertyEditorBase(prop),
  value_(nullptr)
{
  value_ = static_cast<assets::Mesh::Bounds*>(prop_->get());
  addProperty("Min", &value_->min, prop_->readOnly());
  addProperty("Max", &value_->max, prop_->readOnly());
  editors[0] = std::make_unique<Vec3PropertyEditor>(properties_[0].get());
  editors[1] = std::make_unique<Vec3PropertyEditor>(properties_[1].get());
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshBoundsPropertyEditor::render()
{
  editors[0]->render();
  editors[1]->render();
}

// -----------------------------------------------------------------------------------------------------------------------------
QuatPropertyEditor::QuatPropertyEditor(IProperty* prop) :
  Vec3PropertyEditor(prop),
  values_{}
{
  label_[0] = "Pitch";
  label_[1] = "Roll";
  label_[2] = "Yaw";
  inputName_[0] = "##Pitch#Vec3PropertyEditor" + prop_->name();
  inputName_[1] = "##Roll#Vec3PropertyEditor"  + prop_->name();
  inputName_[2] = "##Yaw#Vec3PropertyEditor"   + prop_->name();
  connections_.push_back(prop_->onUpdate([this](void* sender) { updateValues(sender); }));
  updateValues(nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------------
void QuatPropertyEditor::render()
{
  const auto& style = ImGui::GetStyle();
  const float windowWidth = ImGui::GetContentRegionAvailWidth();
  const float width = (windowWidth - sizes_[0] - sizes_[1] - sizes_[2] - 5.0f * style.ItemSpacing.x) / 3.0f;
  bool result = false;
  ImGui::TextUnformatted(prop_->name().c_str());
  for (size_t i = 0; i < 3; ++i) {
    result |= renderInput(i, values_[i], width);
    ImGui::SameLine();
  }
  ImGui::NewLine();
  if (result) {
    auto quat = glm::quat_cast(glm::orientate4(glm::vec3(
      values_[0] * glm::pi<float>() / 180.0f,
      values_[1] * glm::pi<float>() / 180.0f,
      values_[2] * glm::pi<float>() / 180.0f
    )));
    prop_->set(&quat, this);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void QuatPropertyEditor::updateValues(void* sender)
{
  if (sender == this) return;
  auto values = glm::eulerAngles(*static_cast<glm::quat*>(prop_->get()));
  values_[0] = values.x * 180.0f / glm::pi<float>();
  values_[1] = values.y * 180.0f / glm::pi<float>();
  values_[2] = values.z * 180.0f / glm::pi<float>();
}

} // !namespace yaga
