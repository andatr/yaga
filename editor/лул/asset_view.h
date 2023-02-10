#ifndef YAGA_EDITOR_SRC_ASSET_VIEW
#define YAGA_EDITOR_SRC_ASSET_VIEW

#include <memory>

#include "asset_view_data.h"
#include "gui/context.h"

namespace yaga {
namespace editor {

class AssetViewImpl;

class AssetView
{
public:
  AssetView(GuiContext context, AssetViewData* data);

private:
  std::shared_ptr<AssetViewImpl> impl_;
};

typedef std::unique_ptr<AssetView> AssetViewPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_VIEW
