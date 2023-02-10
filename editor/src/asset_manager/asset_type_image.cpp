#include "precompiled.h"
#include "asset_type_image.h"
#include "assets/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace yaga {
namespace editor {
namespace {

// -----------------------------------------------------------------------------------------------------------------------------
int readStbi(void* user, char* data, int size)
{
  auto stream = static_cast<std::istream*>(user);
  return static_cast<int>(stream->read(data, size).gcount());
}

// -----------------------------------------------------------------------------------------------------------------------------
void skipStbi(void* user, int n)
{
  auto stream = static_cast<std::istream*>(user);
  if (n < 0) {
    stream->seekg(-n, std::ios::cur);
  }
  else {
    stream->ignore(n);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
int eofStbi(void* user)
{
  auto stream = static_cast<std::istream*>(user);
  return stream->good() ? 0 : 1;
}

} // !namespace

// -----------------------------------------------------------------------------------------------------------------------------
AssetTypeImage::AssetTypeImage() :
  AssetType(assets::StandardAssetType::image, "image", "yimg", true)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
assets::AssetPtr AssetTypeImage::createAsset(const std::string& name, const std::string& filename)
{
  std::ifstream stream(filename, std::ios::in | std::ios::binary);
  const stbi_io_callbacks callbacks = { readStbi, skipStbi, eofStbi };
  int width = 0, height = 0, channels = 0;
  auto bytes = stbi_load_from_callbacks(&callbacks, &stream, &width, &height, &channels, STBI_rgb_alpha);
  if (!bytes) THROW("Could not deserialize image");
  auto image = std::make_unique<assets::Image>(name);
  image->width(static_cast<uint32_t>(width));
  image->height(static_cast<uint32_t>(height));
  image->format(channels);
  image->bytes([width, height, channels, src = bytes](assets::Image::Bytes& bytes) {
    bytes.resize(static_cast<size_t>(width) * height * channels);
    memcpy(bytes.data(), src, bytes.size());
    stbi_image_free(src);
  });
  return image;
}

} // !namespace editor
} // !namespace yaga
