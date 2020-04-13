#ifndef YAGA_VULKAN_RENDERER_SRC_SHADER
#define YAGA_VULKAN_RENDERER_SRC_SHADER

#include <memory>

#include <boost/noncopyable.hpp>

#include "device.h"
#include "vulkan.h"
#include "engine/asset/shader.h"
#include "utility/auto_destructor.h"

namespace yaga
{
namespace vk
{

class Shader : private boost::noncopyable
{
public:
  Shader(Device* device, asset::Shader* asset);
  VkShaderModule operator*() const { return *shader_; }
private:
  AutoDestructor<VkShaderModule> shader_;
};

typedef std::unique_ptr<Shader> ShaderPtr;

} // !namespace vk
} // !namespace yaga

#endif // !YAGA_VULKAN_RENDERER_SRC_SHADER
