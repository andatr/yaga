#ifndef YAGA_EDITOR_SRC_ASSET_VIEW_DATA
#define YAGA_EDITOR_SRC_ASSET_VIEW_DATA

#include <map>
#include <set>
#include <string>

#include "assets/asset.h"
#include "utility/prop_info.h"

namespace yaga {
namespace editor {

class AssetViewData : public PropInfo
{
public:
  typedef std::map<assets::AssetType, std::set<std::string>> Assets;
  struct PropertyIndex
  {
    static const int assets = 0;
  };

public:
  virtual ~AssetViewData() {}
  virtual const Assets& assets() const = 0;
};

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_ASSET_VIEW_DATA
