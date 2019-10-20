#ifndef YAGA_ENGINE_PIPELINE
#define YAGA_ENGINE_PIPELINE

#include <vector>
#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "Array.h"
#include "AutoDeleter.h"
#include "Asset/Material.h"

namespace yaga
{
	class Device;
	class VideoBuffer;

	class Material : private boost::noncopyable
	{
	public:
		Material(Device* device, VideoBuffer* videoBuffer, VkCommandPool commandPool, asset::Material* asset);
		const std::vector<VkCommandBuffer>& CommandBuffers() const { return _commandBuffers; }
	private:
		void CreatePipeline(Device* device, VideoBuffer* videoBuffer, asset::Material* asset);
		void CreateLayout(VkDevice device);
		void CreateRenderPass(VkDevice device, VkFormat imageFormat);		
		void CreateFramebuffers(VkDevice device, VideoBuffer* videoBuffer);
		void CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer);
	private:
		AutoDeleter<VkRenderPass> _renderPass;
		AutoDeleter<VkPipelineLayout> _layout;
		AutoDeleter<VkPipeline> _pipeline;
		Array<AutoDeleter<VkFramebuffer>> _frameBuffers;
		std::vector<VkCommandBuffer> _commandBuffers;
	};
}

#endif // !YAGA_ENGINE_PIPELINE
