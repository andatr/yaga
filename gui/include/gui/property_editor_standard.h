#ifndef YAGA_GUI_PROPERTY_EDITOR_STANDARD
#define YAGA_GUI_PROPERTY_EDITOR_STANDARD

#include <string>
#include <vector>

#include "imgui_wrapper.h"
#include "assets/mesh.h"
#include "gui/property_editor.h"
#include "utility/glm.h"
#include "utility/prop_info.h"
#include "utility/property.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class PropertyEditorCategory : public PropertyEditor
{
public:
  typedef PropInfo Type;

public:
  explicit PropertyEditorCategory(IProperty* prop);
  explicit PropertyEditorCategory(PropInfo* propInfo);
  void render() override;

protected:
  IProperty* prop_;
  bool root_;
  std::vector<PropertyEditorPtr> editors_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class PropertyEditorBase : public PropertyEditor
{
public:
  explicit PropertyEditorBase(IProperty* prop);
  virtual ~PropertyEditorBase() {};

protected:
  IProperty* prop_;
  ImGuiInputTextFlags flags_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class StringPropertyEditor : public PropertyEditorBase
{
public:
  typedef std::string Type;

public:
  explicit StringPropertyEditor(IProperty* prop);
  void render() override;

private:
  std::string name_;
  std::vector<char> buffer_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class IntPropertyEditor : public PropertyEditorBase
{
public:
  typedef int64_t Type;

public:
  explicit IntPropertyEditor(IProperty* prop);
  void render() override;

private:
  std::string name_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class Vec3PropertyEditor : public PropertyEditorBase
{
public:
  typedef glm::vec3 Type;

public:
  explicit Vec3PropertyEditor(IProperty* prop);
  virtual ~Vec3PropertyEditor() {}
  void render() override;

protected:
  bool renderInput(size_t index, float& value, float width);

protected:
  std::string label_[3];
  std::string inputName_[3];
  float sizes_[3];
};

// -----------------------------------------------------------------------------------------------------------------------------
class QuatPropertyEditor : public Vec3PropertyEditor
{
public:
  typedef glm::quat Type;

public:
  explicit QuatPropertyEditor(IProperty* prop);
  void render() override;

private:
  void updateValues(void* sender);

private:
  float values_[3];
  SignalConnections connections_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class MeshBoundsPropertyEditor :
  public PropertyEditorBase,
  public PropInfo
{
public:
  typedef assets::Mesh::Bounds Type;

public:
  explicit MeshBoundsPropertyEditor(IProperty* prop);
  virtual ~MeshBoundsPropertyEditor() {}
  void render() override;

private:
  assets::Mesh::Bounds* value_;
  PropertyEditorPtr editors[2];
};

} // !namespace yaga

#endif // !YAGA_GUI_PROPERTY_EDITOR_STANDARD
