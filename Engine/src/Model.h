#ifndef YAGA_ENGINE_MODEL
#define YAGA_ENGINE_MODEL

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Material.h"

namespace yaga
{
	class Model : private boost::noncopyable
	{
	public:
		Model(std::unique_ptr<Material> material, std::unique_ptr<Mesh> mesh, VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer);
		const std::vector<VkCommandBuffer>& CommandBuffers() const { return _commandBuffers; }
	private:
		void CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer);
	private:
		std::unique_ptr<Material> _material;
		std::unique_ptr<Mesh> _mesh;
		std::vector<VkCommandBuffer> _commandBuffers;
	};
}

#endif // !YAGA_ENGINE_MODEL
