#include "precompiled.h"
#include "list_view.h"
#include "rename_dialog.h"
#include "gui/file_dialog.h"
#include "gui/popup.h"
#include "gui/widget.h"

#define ICON_SIZE (ImGui::GetFontSize() + 4.0f)

namespace yaga {
namespace editor {
namespace {

constexpr float importWidth = 120.0f;
const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

// -----------------------------------------------------------------------------------------------------------------------------
struct Button
{
  std::string label;
  bool hovered;
  bool active;

  Button(const std::string& label = "");
};

// -----------------------------------------------------------------------------------------------------------------------------
struct Entry
{
  Button btnDelete;
  Button btnRename; 

  Entry(const std::string& name, size_t index);
};

// -----------------------------------------------------------------------------------------------------------------------------
Button::Button(const std::string& label) :
  label(label),
  hovered(false),
  active(false)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
Entry::Entry(const std::string& name, size_t index)
{
  const std::string i = std::to_string(index);
  btnDelete.label = "\xef\x87\xb8##" + name + "##ButtonDelete" + i;
  btnRename.label = "\xef\x81\x84##" + name + "##ButtonRename" + i;
}

} // !namespace

class ListViewImpl : public Widget
{
public:
  ListViewImpl(GuiContext context, const std::string& name, ListViewData* data);
  void render() override;

private:
  void renderTable();
  void renderTableRow(size_t index);
  bool renderButton(Button& button);
  void update();
  std::string makeLabel(const std::string& label);
  std::string makeId(const std::string& id);
  void importClick();
  void removeClick(ListViewEntry* entry, size_t index);
  void renameClick(ListViewEntry* entry, size_t index);

private:
  std::string name_;
  ListViewData* list_;
  std::vector<Entry> entries_;
  ImVec2 importSize_;
  ImVec2 windowSize_;
  std::string importLabel_;
  std::string listId_;
  std::string deleteId_;
  std::vector<SignalConnectionPtr> connections_;
};

// -----------------------------------------------------------------------------------------------------------------------------
ListViewImpl::ListViewImpl(GuiContext context, const std::string& name, ListViewData* list) :
  Widget(context),
  name_(name),
  list_(list),
  importSize_{},
  windowSize_{}
{
  importLabel_ = makeLabel("Import");
  listId_      = makeId("ModelList");
  deleteId_    = makeId("DeleteConfirm");
  connections_.push_back(list_->properties(ListViewData::PropertyIndex::entries)->onUpdate([this](void*) { update(); }));
  docking(Docking::Left);
  update();
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::render()
{
  setPosition();
  if (ImGui::Begin("Models", nullptr, ImGuiWindowFlags_NoCollapse)) {
    windowSize_ = ImGui::GetContentRegionAvail();
    renderTable();
    if (ImGui::Button(importLabel_.c_str(), ImVec2(importWidth, 0.0f))) {
      importClick();
    }
    importSize_ = ImGui::GetItemRectSize();
  }
  updatePosition();
  ImGui::End();
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::renderTable()
{
  const auto& style = ImGui::GetStyle();
  const auto tableSize = windowSize_ - ImVec2(0.0f, importSize_.y + style.ItemSpacing.y);
  if (ImGui::BeginChild(listId_.c_str(), tableSize, true, ImGuiWindowFlags_HorizontalScrollbar)) {
    for (size_t i = 0; i < entries_.size(); ++i) {
      renderTableRow(i);
    }
  }
  ImGui::EndChild();
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::renderTableRow(size_t index)
{
  const auto& style = ImGui::GetStyle();
  const auto size   = ImVec2(ImGui::GetWindowContentRegionWidth() - 2.0f * (ICON_SIZE + style.ItemSpacing.x), 0.0f);
  auto* entry = list_->entries()[index];
  auto& buttons = entries_[index];
  if (ImGui::Selectable(entry->name().c_str(), index == list_->selected(), ImGuiSelectableFlags_AllowDoubleClick, size)) {
    context_.render([this, index]() { list_->select(index); });
  }
  ImGui::SameLine();
  if (renderButton(buttons.btnRename)) {
    renameClick(entry, index);
  }
  ImGui::SameLine();
  if (renderButton(buttons.btnDelete)) {
    removeClick(entry, index);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
bool ListViewImpl::renderButton(Button& button)
{
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
  if (button.active) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }
  else if(button.hovered) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }
  else {
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));
  }
  ImGui::PushStyleColor(ImGuiCol_Button,        transparent);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, transparent);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive,  transparent);
  bool result = ImGui::Button(button.label.c_str(), ImVec2(ICON_SIZE, ICON_SIZE));
  button.active  = ImGui::IsItemActive();
  button.hovered = ImGui::IsItemHovered();
  ImGui::PopStyleColor(4);
  ImGui::PopStyleVar();
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::update()
{
  entries_.clear();
  entries_.reserve(list_->entries().size());
  for (size_t i = 0; i < list_->entries().size(); ++i) {
    entries_.emplace_back(name_, i);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string ListViewImpl::makeLabel(const std::string& label)
{
  return label + "##" + name_;
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string ListViewImpl::makeId(const std::string& id)
{
  return std::string("##") + name_ + "##" + id;
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::importClick()
{
  struct State
  {
    bool show;
    bool result;
    std::string path;
    std::vector<assets::AssetPtr> assets;
    State(const std::string& p) : show(true), result(false), path(p) {}
  };
  file_dialog::render(context_, file_dialog::Mode::Open, [this](const std::string& path) {
    auto state = std::make_shared<State>(path);
    popup::render(context_, "##PopupLoading", "Loading...", [state]() { return state->show; });
    context_.post([state, this]() {
      state->result = list_->import(state->path, state->assets);
    }, [state, this]() {
      state->show = false;
      if (state->result) {
        list_->syncImport(state->assets);
      }
      else {
        auto msg = std::string("Cannot load assets from \"") + state->path + "\"";
        popup::render(context_, "##PopupLoadingError", msg, popup::Result::Cancel);
      }
    });
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::removeClick(ListViewEntry* entry, size_t index)
{
  std::string title = "Delete " + entry->name() + "?";
  popup::render(context_, deleteId_.c_str(), title, popup::Result::YesNoCancel, [this, index](popup::Result result) {
    if (result == popup::Result::Yes) {
      auto asset = list_->remove(index);
      context_.post([a = std::move(asset)]() { /* a = nullptr; */ });
    }
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void ListViewImpl::renameClick(ListViewEntry* entry, size_t index)
{
  rename::render(context_, entry->name(), [this, index](const std::string& value) { 
    list_->rename(index, value);
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
ListView::ListView(GuiContext context, const std::string& name, ListViewData* data)
{
  impl_ = std::make_shared<ListViewImpl>(context, name, data);
  impl_->enqueueRendering();
}

} // !namespace editor
} // !namespace yaga
