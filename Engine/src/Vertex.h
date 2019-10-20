#ifndef YAGA_ENGINE_VERTEX
#define YAGA_ENGINE_VERTEX

#include <array>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace yaga
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
	};

	extern const std::array<VkVertexInputBindingDescription, 1> VertexDescription;

	extern const std::array<VkVertexInputAttributeDescription, 2> VertexAttributeDescription;
}

#endif // !YAGA_ENGINE_VERTEX
