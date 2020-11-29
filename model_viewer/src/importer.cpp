#include "precompiled.h"
#include "importer.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
assets::MeshPtr importMesh(const std::string& path, MeshMetadata& meta)
{
  /* auto asset = std::make_unique<assets::Mesh>("keklol");
    meta.min = { -6.0f, -5.0f, -5.0f };
    meta.max = { -5.0f, 5.0f, 5.0f };
    return std::move(asset);
  */
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path.c_str(),
    aiProcess_CalcTangentSpace      |
    aiProcess_Triangulate           |
    aiProcess_JoinIdenticalVertices |
    aiProcess_SortByPType);
  if (!scene) {
    THROW("Cannot read file \"%1%\"", path);
  }
  if (scene->mNumMeshes < 1) {
    THROW("No meshes in file \"%1%\"", path);
  }
  const auto mesh = scene->mMeshes[0];
  auto asset = std::make_unique<assets::Mesh>(path);
  asset->vertices([mesh, &meta](auto& vertices) {
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
      meta.min.x = std::min(mesh->mVertices[i].x, meta.min.x);
      meta.min.y = std::min(mesh->mVertices[i].y, meta.min.y);
      meta.min.z = std::min(mesh->mVertices[i].z, meta.min.z);
      meta.max.x = std::max(mesh->mVertices[i].x, meta.max.x);
      meta.max.y = std::max(mesh->mVertices[i].y, meta.max.y);
      meta.max.z = std::max(mesh->mVertices[i].z, meta.max.z);
    }
  });
  asset->indices([mesh](auto& indices) {
    indices.reserve(mesh->mNumFaces * 3);
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
      for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
        indices.push_back(mesh->mFaces[i].mIndices[j]);
      }
    }
  });
  return std::move(asset);
}

} // !namespace yaga
