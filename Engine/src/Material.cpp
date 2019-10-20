#include "Pch.h"
#include "Material.h"
#include "Device.h"
#include "VideoBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"
#include "Asset/Material.h"

namespace yaga
{
namespace
{
	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineVertexInputStateCreateInfo VertexAttributes()
	{
		VkPipelineVertexInputStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		info.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexDescription.size());
		info.pVertexBindingDescriptions = VertexDescription.data();
		info.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributeDescription.size());
		info.pVertexAttributeDescriptions = VertexAttributeDescription.data();
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineInputAssemblyStateCreateInfo Primitives() 
	{
		VkPipelineInputAssemblyStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		info.primitiveRestartEnable = VK_FALSE;
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkViewport Viewport(const VkExtent2D& resolution)
	{
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)resolution.width;
		viewport.height = (float)resolution.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		return viewport;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkRect2D Scissors(const VkExtent2D& resolution)
	{
		VkRect2D scissors = {};
		scissors.offset = { 0, 0 };
		scissors.extent = resolution;
		return scissors;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineViewportStateCreateInfo ViewportState(const VkViewport& viewport, const VkRect2D& scissors)
	{
		VkPipelineViewportStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		info.viewportCount = 1;
		info.pViewports = &viewport;
		info.scissorCount = 1;
		info.pScissors = &scissors;
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineRasterizationStateCreateInfo Rasterizer()
	{
		VkPipelineRasterizationStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.depthClampEnable = VK_FALSE;
		info.rasterizerDiscardEnable = VK_FALSE;
		info.polygonMode = VK_POLYGON_MODE_FILL;
		info.lineWidth = 1.0f;
		info.cullMode = VK_CULL_MODE_BACK_BIT;
		info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		info.depthBiasEnable = VK_FALSE;
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineMultisampleStateCreateInfo Sampler()
	{
		VkPipelineMultisampleStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.sampleShadingEnable = VK_FALSE;
		info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineColorBlendAttachmentState ColorBlender()
	{
		VkPipelineColorBlendAttachmentState info = {};
		info.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		info.blendEnable = VK_FALSE;
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineColorBlendStateCreateInfo ColorBlendState(const VkPipelineColorBlendAttachmentState& blender)
	{
		VkPipelineColorBlendStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		info.logicOpEnable = VK_FALSE;
		info.logicOp = VK_LOGIC_OP_COPY;
		info.attachmentCount = 1;
		info.pAttachments = &blender;
		info.blendConstants[0] = 0.0f;
		info.blendConstants[1] = 0.0f;
		info.blendConstants[2] = 0.0f;
		info.blendConstants[3] = 0.0f;
		return info;
	}

	// -------------------------------------------------------------------------------------------------------------------------
	VkPipelineShaderStageCreateInfo ShaderStage(VkShaderModule module, VkShaderStageFlagBits shaderType)
	{
		VkPipelineShaderStageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.stage = shaderType;
		info.module = module;
		info.pName = "main";
		return info;
	}

} // !namespace

	// -------------------------------------------------------------------------------------------------------------------------
	Material::Material(Device* device, VideoBuffer* videoBuffer, VkCommandPool commandPool, asset::Material* asset)
	{
		CreatePipeline(device, videoBuffer, asset);
		CreateFramebuffers(device->Logical(), videoBuffer);
		CreateCommandBuffer(device->Logical(), commandPool, videoBuffer);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Material::CreatePipeline(Device* device, VideoBuffer* videoBuffer, asset::Material* asset)
	{
		const auto logicalDevice = device->Logical();

		Shader vertexShader(logicalDevice, asset->VertexShader());
		Shader fragmentShader(logicalDevice, asset->FragmentShader());
		VkPipelineShaderStageCreateInfo shaderStages[] = {
			ShaderStage(vertexShader.ShaderModule(),  VK_SHADER_STAGE_VERTEX_BIT),
			ShaderStage(fragmentShader.ShaderModule(), VK_SHADER_STAGE_FRAGMENT_BIT)
		};
		
		auto vertexAttributes = VertexAttributes();
		auto primitives = Primitives();
		auto viewport = Viewport(videoBuffer->Resolution());
		auto scissors = Scissors(videoBuffer->Resolution());
		auto viewportState = ViewportState(viewport, scissors);
		auto rasterizer = Rasterizer();
		auto sampler = Sampler();
		auto colorBlender = ColorBlender();
		auto colorBlendState = ColorBlendState(colorBlender);

		CreateLayout(logicalDevice);
		CreateRenderPass(logicalDevice, videoBuffer->ImageFormat());

		VkGraphicsPipelineCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		info.stageCount = 2;
		info.pStages = shaderStages;
		info.pVertexInputState = &vertexAttributes;
		info.pInputAssemblyState = &primitives;
		info.pViewportState = &viewportState;
		info.pRasterizationState = &rasterizer;
		info.pMultisampleState = &sampler;
		info.pColorBlendState = &colorBlendState;
		info.layout = _layout.Get();
		info.renderPass = _renderPass.Get();
		info.subpass = 0;
		info.basePipelineHandle = VK_NULL_HANDLE;

		auto destroyPipeline = [logicalDevice](auto pipeline) {
			vkDestroyPipeline(logicalDevice, pipeline, nullptr);
			LOG(trace) << "Pipeline deleted";
		};
		VkPipeline pipeline;
		if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
			THROW("Could not create Pipeline");
		}
		_pipeline.Assign(pipeline, destroyPipeline);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Material::CreateLayout(VkDevice device)
	{
		VkPipelineLayoutCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount = 0;
		info.pushConstantRangeCount = 0;

		auto destroyLayout = [device](auto layout) {
			vkDestroyPipelineLayout(device, layout, nullptr);
			LOG(trace) << "Pipeline Layout deleted";
		};
		VkPipelineLayout layout;
		if (vkCreatePipelineLayout(device, &info, nullptr, &layout) != VK_SUCCESS) {
			THROW("Could not create Pipeline Layout");
		}
		_layout.Assign(layout, destroyLayout);
		LOG(trace) << "Pipeline Layout created";
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Material::CreateRenderPass(VkDevice device, VkFormat imageFormat)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = imageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		auto destroyRenderPass = [device](auto renderPass) {
			vkDestroyRenderPass(device, renderPass, nullptr);
			LOG(trace) << "Render Pass deleted";
		};
		VkRenderPass renderPass;
		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			THROW("Could not create render pass");
		}
		_renderPass.Assign(renderPass, destroyRenderPass);
		LOG(trace) << "Render Pass created";
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Material::CreateFramebuffers(VkDevice device, VideoBuffer* videoBuffer)
	{
		auto destroyFrameBuffer = [device](auto frameBuffer) {
			vkDestroyFramebuffer(device, frameBuffer, nullptr);
			LOG(trace) << "Framebuffer deleted";
		};
		const auto& textures = videoBuffer->Textures();
		_frameBuffers.Resize(textures.size());
		for (size_t i = 0; i < textures.size(); i++)
		{
			VkImageView attachments[] = { textures[i]->ImageView() };
			VkFramebufferCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.renderPass = _renderPass.Get();
			info.attachmentCount = 1;
			info.pAttachments = attachments;
			info.width = videoBuffer->Resolution().width;
			info.height = videoBuffer->Resolution().height;
			info.layers = 1;
			VkFramebuffer frameBuffer;
			if (vkCreateFramebuffer(device, &info, nullptr, &frameBuffer) != VK_SUCCESS) {
				THROW("Could not create framebuffer");
			}
			_frameBuffers[i].Assign(frameBuffer, destroyFrameBuffer);
		}
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Material::CreateCommandBuffer(VkDevice device, VkCommandPool commandPool, VideoBuffer* videoBuffer)
	{
		_commandBuffers.resize(_frameBuffers.Size());
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
			renderPassInfo.renderPass = _renderPass.Get();
			renderPassInfo.framebuffer = _frameBuffers[i].Get();
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = videoBuffer->Resolution();
			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(command, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.Get());
			vkCmdDraw(command, 3, 1, 0, 0);
			vkCmdEndRenderPass(command);

			if (vkEndCommandBuffer(command) != VK_SUCCESS) {
				THROW("Failed to record command buffer");
			}
		}
	}
}