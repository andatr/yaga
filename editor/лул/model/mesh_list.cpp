#include "precompiled.h"
#include "mesh_list.h"
#include "assets/material.h"
#include "boundingBox.h"

namespace yaga {
namespace editor {
namespace {

constexpr size_t BAD_INDEX = std::numeric_limits<size_t>::max();

// -----------------------------------------------------------------------------------------------------------------------------
void convertVertices(assets::Mesh* asset, aiMesh* mesh)
{
  asset->vertices([asset, mesh](auto& vertices) {
    auto& bounds = asset->bounds();
    vertices.resize(mesh->mNumVertices);
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
      vertices[i].position.x = mesh->mVertices[i].x;
      vertices[i].position.y = mesh->mVertices[i].y;
      vertices[i].position.z = mesh->mVertices[i].z;
      vertices[i].position.w = 1.0f;
      vertices[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
      if (mesh->mTextureCoords[0]) {
        vertices[i].texture.x = mesh->mTextureCoords[0][i].x;
        vertices[i].texture.y = 1.0f - mesh->mTextureCoords[0][i].y;
      }
      bounds.min.x = std::min(mesh->mVertices[i].x, bounds.min.x);
      bounds.min.y = std::min(mesh->mVertices[i].y, bounds.min.y);
      bounds.min.z = std::min(mesh->mVertices[i].z, bounds.min.z);
      bounds.max.x = std::max(mesh->mVertices[i].x, bounds.max.x);
      bounds.max.y = std::max(mesh->mVertices[i].y, bounds.max.y);
      bounds.max.z = std::max(mesh->mVertices[i].z, bounds.max.z);
    }
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
void convertIndices(assets::Mesh* asset, aiMesh* mesh)
{
  asset->indices([mesh](auto& indices) {
    indices.reserve(static_cast<size_t>(mesh->mNumFaces) * 3);
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
      for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
        indices.push_back(mesh->mFaces[i].mIndices[j]);
      }
    }
  });
}

// -----------------------------------------------------------------------------------------------------------------------------
std::string makeUniqueName(assets::Storage* storage, const std::string& base, size_t& index)
{
  if (index == 0) {
    if (!storage->tryGet(base)) return base;
    ++index;
  }
  std::string name = base + std::to_string(index);
  while (storage->tryGet(base)) {
    name = base + std::to_string(++index);
  }
  return name;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
class MeshListEntry : public IListViewEntry
{
public:
  MeshListEntry(assets::Mesh* asset);
  const std::string& name() override { return name_; }
  void name(const std::string& value) { name_ = value; }
  assets::Transform* transform() const { return transform_.get(); }
  assets::Mesh* asset() const { return asset_; }

private:
  assets::Mesh* asset_;
  std::string name_;  
  assets::TransformPtr transform_;
};

// -----------------------------------------------------------------------------------------------------------------------------
MeshListEntry::MeshListEntry(assets::Mesh* asset) :
  asset_(asset),
  name_(asset->name())
{
  const auto& bounds = asset->bounds();
  glm::vec3 size = bounds.max - bounds.min;
  auto max = 10.0f / std::max(std::max(size.x, size.y), size.z);
  glm::vec3 mid = (bounds.max + bounds.min) / 2.0f;
  transform_ = std::make_unique<assets::Transform>("");
  transform_->scale({ max, max, max });
  transform_->translation(-mid);
  addProperty("Mesh", asset);
  addProperty("Transform", transform_.get());
}

struct MeshList::Scene
{
  ObjectPtr object;
  ObjectPtr frame;
  ObjectPtr cameraObject;
  Camera*   camera;
  assets::Material* material;
  assets::Material* frameMaterial;
  assets::CameraPtr cameraAsset;
  assets::TransformPtr cameraTransform;
  Scene() : camera(nullptr), material(nullptr), frameMaterial(nullptr) {}
};

// -----------------------------------------------------------------------------------------------------------------------------
MeshList::MeshList(Context* context, assets::Serializer* serializer, assets::Storage* storage) :
  context_(context),
  serializer_(serializer),
  storage_(storage),
  selected_(BAD_INDEX)
{
  scene_ = std::make_unique<MeshList::Scene>();
  scene_->material      = serializer_->deserialize<assets::Material>("mlist", storage_);
  scene_->frameMaterial = serializer_->deserialize<assets::Material>("ui_3d_mat", storage_);
  addProperty("Entries", &entriesRef_);
  addProperty("Selected", &selected_);
  setupCamera();
}

// -----------------------------------------------------------------------------------------------------------------------------
bool MeshList::import(const std::string& path, std::vector<assets::AssetPtr>& assets)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path.c_str(),
    aiProcess_CalcTangentSpace      |
    aiProcess_Triangulate           |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType);
  if (!scene || scene->mNumMeshes < 1) {
    LOG(error) << "Error reading file \"" << path << "\"";
    return false;
  }
  size_t index = 0;
  auto basename = boost::filesystem::path(path).stem().string();
  for (size_t i = 0; i < scene->mNumMeshes; ++i) {
    const auto mesh = scene->mMeshes[i];
    auto name = makeUniqueName(storage_, basename, index);
    auto asset = std::make_unique<assets::Mesh>(name);
    convertVertices(asset.get(), mesh);
    convertIndices (asset.get(), mesh);
    auto frame = makeBoundingBox(asset.get());
    assets.push_back(std::move(asset));
    assets.push_back(std::move(frame));
  }
  return true;
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::syncImport(std::vector<assets::AssetPtr>& assets)
{
  for (size_t i = 0; i < assets.size(); ++i) {
    auto ptr = assets[i].get();
    storage_->put(std::move(assets[i]));
    auto entry = std::make_unique<MeshListEntry>(static_cast<assets::Mesh*>(ptr));
    entriesRef_.push_back(entry.get());
    entries_.push_back(std::move(entry));
  }
  if (selected_ == BAD_INDEX) {
    select(0);
  }
  properties_[PropertyIndex::entries]->update(this);
  assets.clear();
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::select(size_t index)
{
  if (index >= entries_.size()) return;
  selected_ = index;
  auto entry = entries_[index].get();
  scene_->object = std::make_unique<Object>();
  auto mesh = storage_->get<assets::Mesh>(entry->name());
  scene_->object->addComponent(context_->createMesh(mesh));
  scene_->object->addComponent(context_->createTransform(entry->transform()));
  scene_->object->addComponent(context_->createMaterial(scene_->material));
  scene_->object->addComponent(context_->createRenderer3D());

  scene_->frame = std::make_unique<Object>();
  mesh = storage_->get<assets::Mesh>(entry->name() + "::frame");
  scene_->frame->addComponent(context_->createMesh(mesh));
  scene_->frame->addComponent(context_->createTransform(entry->transform()));
  scene_->frame->addComponent(context_->createMaterial(scene_->frameMaterial))->wireframe(true);
  scene_->frame->addComponent(context_->createRenderer3D());

  context_->mainCamera(scene_->camera);
  properties_[PropertyIndex::selected]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::deselect()
{
  scene_->object = nullptr;
  scene_->frame  = nullptr;
  selected_ = BAD_INDEX;
  properties_[PropertyIndex::selected]->update(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
size_t MeshList::selected()
{
  return selected_;
}

// -----------------------------------------------------------------------------------------------------------------------------
IListViewEntry* MeshList::selectedEntry()
{
  return selected_ == BAD_INDEX ? nullptr : entriesRef_[selected_];
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr MeshList::remove(size_t index)
{
  if (index >= entries_.size()) return nullptr;
  if (index == selected_) {
    deselect();
  }
  auto asset = entries_[index]->asset();
  entries_.erase(entries_.begin() + index);
  entriesRef_.erase(entriesRef_.begin() + index);
  properties_[PropertyIndex::entries]->update(this);
  return storage_->remove(asset);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::rename(size_t index, const std::string& value)
{
  if (index < entries_.size()) {
    entries_[index]->name(value);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
const std::vector<IListViewEntry*>& MeshList::entries() const
{
  return entriesRef_;
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::setupCamera()
{
  scene_->cameraTransform = std::make_unique<assets::Transform>("");
  scene_->cameraAsset     = std::make_unique<assets::Camera>("");
  scene_->cameraObject    = std::make_unique<Object>();
  scene_->camera          = scene_->cameraObject->addComponent(context_->createCamera(scene_->cameraAsset.get()));
  scene_->cameraObject->addComponent(context_->createTransform(scene_->cameraTransform.get()));
  resize();
  glm::vec3 translate(10.0f * sin(0.0f), 3.0f + 3.0f * sin(0.0f), 10.0f * cos(0.0f));
  scene_->cameraTransform->translation(translate);
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::resize()
{
  const auto& res = context_->resolution();
  auto projection = glm::perspective(glm::radians(45.0f), (float)res.x / (float)res.y, 0.1f, 10000.0f);
  projection[1][1] *= -1;
  scene_->camera->projection(projection);
  scene_->cameraAsset->lookAt({ 0.0f, 0.0f, 0.0f });
}

// -----------------------------------------------------------------------------------------------------------------------------
void MeshList::update()
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  glm::vec3 translate(10.0f * sin(0.1f * time), 3.0f + 3.0f * sin(0.1f * time), 10.0f * cos(0.1f * time));
  //scene_->cameraTransform->translation(translate);
}

} // !namespace editor
} // !namespace yaga
