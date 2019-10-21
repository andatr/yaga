#include "Pch.h"
#include "Mesh.h"
#include "Device.h"

namespace yaga
{
namespace
{
} // !namespace

	// -------------------------------------------------------------------------------------------------------------------------
	Mesh::Mesh(Device* device) : _device(device)
	{
		_vertices = {
			{{  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
			{{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
			{{ -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }}
		};
		Rebuild();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Mesh::Rebuild()
	{
		VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.size = sizeof(Vertex) * _vertices.size();
		info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		const auto device = _device->Logical();
		auto deleteBuffer = [device](auto buffer) {
			vkDestroyBuffer(device, buffer, nullptr);
			LOG(trace) << "Vertex buffer deleted";
		};
		VkBuffer buffer;
		if (vkCreateBuffer(device, &info, nullptr, &buffer) != VK_SUCCESS) {
			THROW("Could not create vertex buffer");
		}
		_buffer.Assign(buffer, deleteBuffer);
		AllocateMemory(info.size);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	void Mesh::AllocateMemory(VkDeviceSize size)
	{
		const auto device = _device->Logical();
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device, *_buffer, &requirements);

		VkMemoryAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info.allocationSize = requirements.size;
		info.memoryTypeIndex = _device->GetMemoryType(requirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		auto freeMemory = [device](auto memory) {
			vkFreeMemory(device, memory, nullptr);
			LOG(trace) << "Vertex buffer memory released";
		};
		VkDeviceMemory memory;
		if (vkAllocateMemory(device, &info, nullptr, &memory) != VK_SUCCESS) {
			THROW("Could not allocate memory for vertex buffer");
		}
		_memory.Assign(memory, freeMemory);

		void* data;
		vkBindBufferMemory(device, *_buffer, *_memory, 0);
		vkMapMemory(device, *_memory, 0, size, 0, &data);
		memcpy(data, _vertices.data(), static_cast<size_t>(size));
		vkUnmapMemory(device, *_memory);
	}
}