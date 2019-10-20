#ifndef YAGA_ENGINE_SHADER
#define YAGA_ENGINE_SHADER

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "AutoDeleter.h"
#include "Asset/Shader.h"

namespace yaga
{
	class Shader : private boost::noncopyable
	{
	public:
		Shader(VkDevice device, asset::Shader* asset);
		VkShaderModule ShaderModule() const { return _shader.Get(); }
	private:
		AutoDeleter<VkShaderModule> _shader;
	};
}

#endif // !YAGA_ENGINE_SHADER
