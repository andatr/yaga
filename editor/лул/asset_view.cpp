#include "precompiled.h"
#include "asset_view.h"
#include "asset_registry.h"
#include "assets/camera.h"
#include "assets/image.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/model.h"
#include "assets/scene.h"
#include "assets/shader.h"
#include "assets/texture.h"
#include "assets/transform.h"
#include "gui/widget.h"

namespace yaga {
namespace editor {

ASSET_REGISTRY("Cameras",    assets::Camera)
ASSET_REGISTRY("Images",     assets::Image)
ASSET_REGISTRY("Materials",  assets::Material)
ASSET_REGISTRY("Meshes",     assets::Mesh)
ASSET_REGISTRY("Models",     assets::Model)
ASSET_REGISTRY("Scenes",     assets::Scene)
ASSET_REGISTRY("Shaders",    assets::Shader)
ASSET_REGISTRY("Textures",   assets::Texture)
ASSET_REGISTRY("Transforms", assets::Transform)

namespace {

} // !namespace

class AssetViewImpl : public Widget
{
public:
  AssetViewImpl(GuiContext context, AssetViewData* data);
  void render() override;

private:
  struct TreeNode
  {
    typedef AssetViewData::Assets::value_type::second_type::const_iterator Iter;

    std::string name;
    std::vector<std::string> items;
    TreeNode(const std::string& n);
    TreeNode(const std::string& n, Iter b, Iter e);
  };

private:
  void update();

private:
  AssetViewData* assetManager_;
  SignalConnections connections_;
  std::vector<TreeNode> nodes_;
};

// -----------------------------------------------------------------------------------------------------------------------------
AssetViewImpl::TreeNode::TreeNode(const std::string& n) :
  name(n)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetViewImpl::TreeNode::TreeNode(const std::string& n, Iter b, Iter e) :
  name(n),
  items(b, e)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetViewImpl::AssetViewImpl(GuiContext context, AssetViewData* data) :
  Widget(context),
  assetManager_(data)
{
  docking(Docking::Left);
  connections_.push_back(data->properties(AssetViewData::PropertyIndex::assets)->onUpdate([this](void*) { update(); }));
  update();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetViewImpl::render()
{
  setPosition();
  if (ImGui::Begin("Assets##AssetView", nullptr, ImGuiWindowFlags_NoCollapse)) {
    for (size_t i = 0; i < nodes_.size(); ++i) {
      const auto& node = nodes_[i];
      if (ImGui::TreeNode((void*)(intptr_t)i, node.name.c_str())) {
        for (const auto& item : node.items) {
          ImGui::Text(item.c_str());
        }
        ImGui::TreePop();
      }
    }
  }
  updatePosition();
  ImGui::End();
  enqueueRendering();
}

// -----------------------------------------------------------------------------------------------------------------------------
void AssetViewImpl::update()
{
  nodes_.clear();
  const auto& types  = AssetRegistry::instance().assets();
  const auto& assets = assetManager_->assets();
  for (auto typeIter = types.begin(); typeIter != types.end(); ++typeIter) {
    auto assetIter = assetManager_->assets().find(typeIter->first);
    if (assetIter != assets.end()) {
      nodes_.emplace_back(typeIter->second.name, assetIter->second.begin(), assetIter->second.end());
    }
    else {
      nodes_.emplace_back(typeIter->second.name);
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
AssetView::AssetView(GuiContext context, AssetViewData* data)
{
  impl_ = std::make_shared<AssetViewImpl>(context, data);
  impl_->enqueueRendering();
}

} // !namespace editor
} // !namespace yaga
