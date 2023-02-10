#ifndef YAGA_EDITOR_SRC_MESH_LIST
#define YAGA_EDITOR_SRC_MESH_LIST

#include <memory>

#include "list_view_data.h"
#include "assets/mesh.h"
#include "assets/serializer.h"
#include "assets/transform.h"
#include "engine/camera.h"
#include "engine/context.h"
#include "engine/object.h"

namespace yaga {
namespace editor {

class MeshListEntry;
typedef std::unique_ptr<MeshListEntry> MeshListEntryPtr;

class MeshList : public ListViewData
{
public:
  MeshList(Context* context, assets::Serializer* serializer, assets::Storage* storage);
  bool import(const std::string& path, std::vector<assets::AssetPtr>& assets) override;
  void syncImport(std::vector<assets::AssetPtr>& assets) override;
  void select(size_t index) override;
  void deselect() override;
  size_t selected() override;
  ListViewEntry* selectedEntry();
  assets::AssetPtr remove(size_t index) override;
  void rename(size_t index, const std::string& value) override;
  const std::vector<ListViewEntry*>& entries() const override;
  void resize();
  void update();

private:
  void setupCamera();

private:
  struct Scene;
  typedef std::unique_ptr<Scene> ScenePtr;

private:
  Context* context_;
  assets::Serializer* serializer_;
  assets::Storage* storage_;
  ScenePtr scene_;
  std::vector<MeshListEntryPtr> entries_;
  std::vector<ListViewEntry*> entriesRef_;
  size_t selected_;
};

typedef std::unique_ptr<MeshList> MeshListPtr;

} // !namespace editor
} // !namespace yaga

#endif // !YAGA_EDITOR_SRC_MESH_LIST
