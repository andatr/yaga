#ifndef YAGA_VULKAN_ENGINE_CONFIG
#define YAGA_VULKAN_ENGINE_CONFIG

#include "utility/compiler.h"

#include <memory>
#include <string>
#include <glm/glm.hpp>

DISABLE_WARNINGS
#include <boost/property_tree/ptree.hpp>
ENABLE_WARNINGS

#include "utility/options.h"
#include "utility/log.h"

namespace yaga {

class Config
{
public:

  typedef boost::property_tree::ptree ptree;

public:

  class Window
  {
  public:
    Window();
    explicit Window(const ptree& properties);
    void save(ptree& properties);
    bool   fullscreen() const { return fullscreen_; }
    uint32_t    width() const { return width_;      }
    uint32_t   height() const { return height_;     }
    std::string title() const { return title_;      }
    void fullscreen(bool value);
    void width (uint32_t value);
    void height(uint32_t value);
    void title (const std::string& value);

  private:
    bool        fullscreen_;
    uint32_t    width_;
    uint32_t    height_;
    std::string title_;
  };

  class Resources
  {
  public:
    Resources();
    explicit Resources(const ptree& properties);
    void save(ptree& properties);
    uint32_t maxTextureCount() const { return maxTextureCount_; }
    uint64_t    maxImageSize() const { return maxImageSize_;    }
    uint32_t   maxIndexCount() const { return maxIndexCount_;   }
    uint32_t  maxVertexCount() const { return maxVertexCount_;  }
    void maxTextureCount(uint32_t value);
    void maxImageSize   (uint64_t value);
    void maxIndexCount  (uint32_t value);
    void maxVertexCount (uint32_t value);

  private:
    uint32_t maxTextureCount_;
    uint64_t maxImageSize_;
    uint32_t maxIndexCount_;
    uint32_t maxVertexCount_;
  };

  class Rendering
  {
  public:
    Rendering();
    explicit Rendering(const ptree& properties);
    void save(ptree& properties);
    int         msaaLevel() const { return msaaLevel_;    }
    bool      depthBuffer() const { return depthBuffer_;  }
    glm::vec4  clearColor() const { return clearColor_;   }
    float      clearDepth() const { return clearDepth_;   }
    uint32_t clearStencil() const { return clearStencil_; }
    void msaaLevel  (int   value);
    void depthBuffer(bool  value);
    void clearColor (const glm::vec4& value);
    void clearDepth (float value);
    void clearStencil(uint32_t value);

  private:
    int       msaaLevel_;
    bool      depthBuffer_;
    glm::vec4 clearColor_;
    float     clearDepth_;
    uint32_t  clearStencil_;
  };

  class Gui
  {
  public:
    Gui();
    explicit Gui(const ptree& properties);
    void save(ptree& properties);
    glm::vec4 clearColor() const { return clearColor_; }
    const std::string& imGuiSettings() const { return imGuiSettings_; }
    void imGuiSettings(const std::string& value) { imGuiSettings_ = value; }

  private:
    glm::vec4 clearColor_;
    std::string imGuiSettings_;
  };

  class Logging
  {
  public:
    Logging();
    explicit Logging(const ptree& properties);
    void save(ptree& properties);
    log::Severity severity() const { return severity_; }

  private:
    log::Severity severity_;
  };

public:
  Config();
  ~Config();
  explicit Config(const ProgramOptions& options);
  void save();
  Window&       window() { return window_;    }
  Resources& resources() { return resources_; }
  Rendering& rendering() { return rendering_; }
  Gui&             gui() { return gui_;       }
  const Window&       window() const { return window_;    }
  const Resources& resources() const { return resources_; }
  const Rendering& rendering() const { return rendering_; }
  const Gui&             gui() const { return gui_;       }
  const Logging&     logging() const { return logging_;   }
  static boost::program_options::options_description getOptions();

private:
  std::string path_;
  ptree properties_;
  Window    window_;
  Resources resources_;
  Rendering rendering_;
  Gui       gui_;
  Logging   logging_;
};

typedef std::shared_ptr<Config> ConfigPtr;

} // !namespace yaga

#endif // !YAGA_VULKAN_ENGINE_CONFIG
