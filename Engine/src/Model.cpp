#include "Pch.h"
#include "Model.h"

namespace yaga
{
namespace
{
} // !namespace

	// -------------------------------------------------------------------------------------------------------------------------
	Model::Model(std::unique_ptr<Material> material, std::unique_ptr<Mesh> mesh, VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer) :
		_material(std::move(material)), _mesh(std::move(mesh))
	{
		CreateCommandBuffer(device, commandPool, videoBuffer);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Model::CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer)
	{
		_commandBuffers.resize(_material->FrameBuffers().size());
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());
		if (vkAllocateCommandBuffers(device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
			THROW("Could not allocate command buffers");
		}
		for (size_t i = 0; i < _commandBuffers.size(); i++)
		{
			const auto& command = _commandBuffers[i];

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			if (vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS) {
				THROW("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _material->RenderPass();
			renderPassInfo.framebuffer = _material->FrameBuffers()[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = _material->Resolution();
			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->Pipeline());
			VkBuffer vertexBuffers[] = { _mesh->Buffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(command, 0, 1, vertexBuffers, offsets);
			vkCmdDraw(command, static_cast<uint32_t>(_mesh->Vertices().size()), 1, 0, 0);
			vkCmdEndRenderPass(command);

			if (vkEndCommandBuffer(command) != VK_SUCCESS) {
				THROW("Failed to record command buffer");
			}
		}
	}
}