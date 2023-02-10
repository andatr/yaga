#ifndef YAGA_EDITOR_SRC_LIST_VIEW_DATA
#define YAGA_EDITOR_SRC_LIST_VIEW_DATA

#include <memory>
#include <string>
#include <vector>

#include "assets/asset.h"
#include "gui/context.h"
#include "utility/prop_info.h"

namespace yaga {
namespace editor {

class ListViewEntry : public PropInfo
{
public:
  virtual ~ListViewEntry() {}
  virtual const std::string& name() = 0;
};

class ListViewData : public PropInfo
{
public:
  struct PropertyIndex
  {
    static const int entries = 0;
    static const int selected = 0;
  };

public:
  virtual ~ListViewData() {};
  virtual bool import(const std::string& path, std::vector<assets::AssetPtr>& assets) = 0;
  virtual void syncImport(std::vector<assets::AssetPtr>& assets) = 0;
  virtual void select(size_t index) = 0;
  virtual void deselect() = 0;
  virtual size_t selected() = 0;
  virtual assets::AssetPtr remove(size_t index) = 0;
  virtual void rename(size_t index, const std::string& value) = 0;
  virtual const std::vector<ListViewEntry*>& entries() const = 0;
};

typedef std::unique_ptr<ListViewEntry> ListViewEntryPtr;
typedef std::unique_ptr<ListViewData> ListViewDataPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_LIST_VIEW_DATA
