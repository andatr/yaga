#include "precompiled.h"
#include "assets/serializer.h"
#include "assets/application.h"
#include "assets/camera.h"
#include "assets/image.h"
#include "assets/material.h"
#include "assets/mesh.h"
#include "assets/model.h"
#include "assets/scene.h"
#include "assets/shader.h"
#include "assets/texture.h"
#include "utility/array.h"

namespace yaga
{
namespace assets
{

// -------------------------------------------------------------------------------------------------------------------------
RefResolver::RefResolver(Storage* storage, Serializer* serializer) :
  storage_(storage), serializer_(serializer)
{
}

// -------------------------------------------------------------------------------------------------------------------------
Asset* RefResolver::getAsset(const std::string& name)
{
  auto asset = storage_->tryGet(name);
  if (asset) return asset;
  auto newAsset = serializer_->deserialize(name, *this);
  auto ptr = newAsset.get();
  storage_->put(std::move(newAsset));
  return ptr;
}

// -------------------------------------------------------------------------------------------------------------------------
void Serializer::registerStandardAssets()
{
  registerAsset<Application>();
  registerAsset<Image>();
  registerAsset<Scene>();
  registerAsset<Shader>();
  registerAsset<Camera>();
  registerAsset<Material>();
  registerAsset<Mesh>();
  registerAsset<Model>();
  registerAsset<Texture>();
}

// -------------------------------------------------------------------------------------------------------------------------
Asset* Serializer::deserialize(const std::string& name, Storage* storage)
{
  RefResolver resolver(storage, this);
  return resolver.getAsset(name);
}

} // !namespace assets
} // !namespace yaga

