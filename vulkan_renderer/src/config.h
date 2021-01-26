#ifndef YAGA_VULKAN_RENDERER_SRC_CONFIG
#define YAGA_VULKAN_RENDERER_SRC_CONFIG

#include <memory>
#include <string>
#include <boost/property_tree/ptree_fwd.hpp>

#include "assets/asset.h"
#include "assets/serializer.h"
#include "utility/array.h"

namespace yaga {
namespace vk {

class Config
{
public:
  explicit Config(const boost::property_tree::ptree& options);
  virtual ~Config();
  bool fullscreen()          const { return fullscreen_;      }
  uint32_t width()           const { return width_;           }
  uint32_t height()          const { return height_;          }
  std::string title()        const { return title_;           }
  uint32_t maxTextureCount() const { return maxTextureCount_; }
  uint64_t maxImageSize()    const { return maxImageSize_;    }
  uint32_t maxIndexCount()   const { return maxIndexCount_;   }
  uint32_t maxVertexCount()  const { return maxVertexCount_;  }
  const char* guiConfigPath() const { return guiConfig_.c_str(); }

private:
  std::string title_;
  bool fullscreen_;
  uint32_t width_;
  uint32_t height_;
  uint32_t maxTextureCount_;
  uint64_t maxImageSize_;
  uint32_t maxIndexCount_;
  uint32_t maxVertexCount_;
  std::string guiConfig_;
};

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_CONFIG
