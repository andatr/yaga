#ifndef YAGA_RENDERER_SRC_SHADER
#define YAGA_RENDERER_SRC_SHADER

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "asset/shader.h"
#include "utility/auto_destroyer.h"

namespace yaga
{

class Shader : private boost::noncopyable
{
public:
  Shader(VkDevice device, asset::Shader* asset);
  VkShaderModule ShaderModule() const { return *shader_; }
private:
  AutoDestroyer<VkShaderModule> shader_;
};

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_SHADER
