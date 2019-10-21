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
		VkRenderPass RenderPass() const { return *_renderPass; }
		VkPipeline Pipeline() const { return *_pipeline; }
		const VkExtent2D& Resolution() const { return _resolution; }
		const std::vector<VkFramebuffer>& FrameBuffers() const { return _frameBufferRefs; }
	private:
		void CreatePipeline(Device* device, VideoBuffer* videoBuffer, asset::Material* asset);
		void CreateLayout(VkDevice device);
		void CreateRenderPass(VkDevice device, VkFormat imageFormat);		
		void CreateFramebuffers(VkDevice device, VideoBuffer* videoBuffer);
	private:
		VkExtent2D _resolution;
		AutoDeleter<VkRenderPass> _renderPass;
		AutoDeleter<VkPipelineLayout> _layout;
		AutoDeleter<VkPipeline> _pipeline;
		Array<AutoDeleter<VkFramebuffer>> _frameBuffers;
		std::vector<VkFramebuffer> _frameBufferRefs;
	};
}

#endif // !YAGA_ENGINE_PIPELINE
