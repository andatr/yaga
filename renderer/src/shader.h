#ifndef YAGA_RENDERER_SRC_SHADER
#define YAGA_RENDERER_SRC_SHADER

#include <memory>

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "device.h"
#include "engine/asset/shader.h"
#include "utility/auto_destructor.h"

namespace yaga
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

} // !namespace yaga

#endif // !YAGA_RENDERER_SRC_SHADER
