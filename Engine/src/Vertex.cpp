#include "Pch.h"
#include "Vertex.h"

namespace yaga
{
namespace
{
} // !namespace

	// std::array does not have a constructor that takes an initializer list.
	// so extra braces for aggregate initialization.

	const std::array<VkVertexInputBindingDescription, 1> VertexDescription = {{{
		/* binding   */ 0,
		/* stride    */ sizeof(Vertex),
		/* inputRate */ VK_VERTEX_INPUT_RATE_VERTEX
	}}};

	const std::array<VkVertexInputAttributeDescription, 2> VertexAttributeDescription = {{{
		/* binding  */ 0,
		/* location */ 0,
		/* format   */ VK_FORMAT_R32G32_SFLOAT,
		/* offset   */ offsetof(Vertex, pos)
	}, {
		/* binding  */ 0,
		/* location */ 1,
		/* format   */ VK_FORMAT_R32G32B32_SFLOAT,
		/* offset   */ offsetof(Vertex, color)
	}}};
}