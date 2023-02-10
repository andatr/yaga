#ifndef YAGA_EDITOR_SRC_EDITOR_GUI_ASSET_VIEW
#define YAGA_EDITOR_SRC_EDITOR_GUI_ASSET_VIEW

#include <memory>

#include "asset_manager/asset_manager.h"
#include "gui/widget.h"

namespace yaga {
namespace editor {

class AssetView : public Widget
{
public:
  AssetView(GuiContext context, AssetManager* assetManager);
  void render() override;

private:
  struct TreeNodeItem
  {
    explicit TreeNodeItem(AssetMetadata* asset);
    std::string name;
  };
  struct TreeNode
  {
    AssetTypeId type;
    std::string name;
    std::string popupId;
    std::vector<TreeNodeItem> items;
    TreeNode(const AssetManager::AssetTypeStruct& info);
  };

private:
  void update();
  void renderGlobalMenu();
  void renderItem(const TreeNodeItem& item);
  void renderNodeMenu(const TreeNode& node);
  void renderNodeMenuBody();
  void openPopupOnMouseClick(const char* name);
  void addAsset();
  void renameAsset();
  void deleteAsset();

private:
  AssetManager* assetManager_;
  SignalConnections connections_;
  std::vector<TreeNode> nodes_;
  const TreeNode* currentNode_;
  const TreeNodeItem* currentItem_;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_GUI_ASSET_VIEW
