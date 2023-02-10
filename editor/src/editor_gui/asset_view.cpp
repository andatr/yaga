#include "precompiled.h"
#include "asset_view.h"
#include "rename_dialog.h"
#include "gui/popup.h"
#include "gui/file_dialog.h"

namespace yaga {
namespace editor {
namespace {

const ImVec2 defaultSize(300.0f, 300.0f);
constexpr const char* GLOBAL_POPUP_ID = "NodePopup##AssetView";
constexpr const char* ITEM_POPUP_ID   = "ItemPopup##AssetView";

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
AssetView::TreeNodeItem::TreeNodeItem(AssetMetadata* asset) :
  name(asset->name())
{
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetView::TreeNode::TreeNode(const AssetManager::AssetTypeStruct& info) :
  type(info.type->id()),
  name(info.type->name()),
  popupId(name + GLOBAL_POPUP_ID)
{
  for (auto& asset: info.assets) {
    items.emplace_back(asset.second.get());
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetView::AssetView(GuiContext context, AssetManager* assetManager) :
  Widget(context),
  assetManager_(assetManager),
  currentNode_(nullptr),
  currentItem_(nullptr)
{
  docking(Docking::Left);
  //connections_.push_back(assetManager_->properties(AssetManagerGui::PropertyIndex::assets)->onUpdate([this](void*) { 
  //  update();
  //}));
  update();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::render()
{
  ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
  setPosition();
  if (ImGui::Begin("Assets##AssetView", nullptr, ImGuiWindowFlags_NoCollapse)) {
    renderGlobalMenu();
    for (size_t i = 0; i < nodes_.size(); ++i) {
      const auto& node = nodes_[i];
      /*if (&node == currentNode_) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
      }*/
      if (ImGui::TreeNode((void*)(intptr_t)i, node.name.c_str())) {
        //if (&node == currentNode_) ImGui::PopStyleColor(2);
        if (ImGui::IsItemHovered()) currentNode_ = &node;
        renderNodeMenu(node);
        for (const auto& item : node.items) {
          renderItem(item);
        }
        ImGui::TreePop();
      }
      else {
        //if (&node == currentNode_) ImGui::PopStyleColor(2);
        if (ImGui::IsItemHovered()) currentNode_ = &node;
        openPopupOnMouseClick(GLOBAL_POPUP_ID);
      }
    }    
  }
  updatePosition();
  ImGui::End();
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::renderItem(const TreeNodeItem& item)
{
  ImGui::Text(item.name.c_str());
  openPopupOnMouseClick(ITEM_POPUP_ID);
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::renderNodeMenu(const TreeNode& node)
{
  if (ImGui::BeginPopup(node.popupId.c_str())) {
    renderNodeMenuBody();
  }
  openPopupOnMouseClick(node.popupId.c_str());
  if (ImGui::BeginPopup(ITEM_POPUP_ID)) {
    if (ImGui::Selectable("Add"))    addAsset();
    if (ImGui::Selectable("Rename")) renameAsset();
    if (ImGui::Selectable("Delete")) deleteAsset();
    ImGui::EndPopup();
  }  
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::renderGlobalMenu()
{
  if (ImGui::BeginPopup(GLOBAL_POPUP_ID)) {
    renderNodeMenuBody();
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::renderNodeMenuBody()
{
  if (ImGui::Selectable("Add")) addAsset();
  ImGui::EndPopup();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::openPopupOnMouseClick(const char* name)
{
  if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)) {
    ImGui::OpenPopup(name);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::addAsset()
{
  if (!currentNode_) return;
  const auto info = assetManager_->assets().find(currentNode_->type);
  if (info == assetManager_->assets().end()) return;
  auto addAssetProc = [this, type = currentNode_->type](const std::string& filename, const std::string& name) {
    //try {
    //  throw 1;
      //assetManager_->createAsset(type, name, filename);
    //}
    //catch (...) {
      auto flag = std::make_shared<bool>(true);

      popup::render(context_, "##qwe", "azazaza", [flag]() { return flag.get(); });
      return false;
    //}
    //return true;
  };
  if (info->second.type->external()) {
    file_dialog::render(context_, file_dialog::Mode::Open, [this, addAssetProc](const std::string& filename) {
      const std::string stem = boost::filesystem::path(filename).stem().string();
      rename::render(context_, "New " + currentNode_->name, stem, std::bind(addAssetProc, filename, std::placeholders::_1));
    });
  }
  else {
    rename::render(context_, "New " + currentNode_->name, "", std::bind(addAssetProc, std::string(), std::placeholders::_1));
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::renameAsset()
{
  /*if (!currentItem_) return;
  auto a = currentItem_->asset;
  rename::render(context_, "Rename " + a->name(), a->name(), [this, a](const std::string& n) {
    if (n.empty()) return false;
    context_.postSeq([a, n]() { a->rename(n); });
    return true;
  */
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::deleteAsset()
{
  /*if (!currentItem_) return;
  std::string content = "Are you sure you want to delete asset \"" + currentItem_->asset->name() + "\"?";
  popup::render(context_, "##Delete##AssetView", content, popup::Result::YesNo,
    [this, a = currentItem_->asset](popup::Result result) { 
      if (result == popup::Result::Yes) context_.postSeq([a]() { a->remove(); });
    });
  */
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetView::update()
{
  nodes_.clear();
  for (auto& kvp: assetManager_->assets()) {
    nodes_.emplace_back(kvp.second);
  }
}

} // !namespace editor
} // !namespace yaga
