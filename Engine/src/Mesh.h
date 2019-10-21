#ifndef YAGA_ENGINE_MESH
#define YAGA_ENGINE_MESH

#include <vector>
#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "Vertex.h"
#include "AutoDeleter.h"

namespace yaga
{
	class Device;

	class Mesh : private boost::noncopyable
	{
	public:
		explicit Mesh(Device* deivce);
		void Rebuild();
		VkBuffer Buffer() const { return *_buffer; }
		const std::vector<Vertex>& Vertices() const { return _vertices; }
	private:
		void AllocateMemory(VkDeviceSize size);
	private:
		std::vector<Vertex> _vertices;
		Device* _device;
		AutoDeleter<VkDeviceMemory> _memory;
		AutoDeleter<VkBuffer> _buffer;
	};
}

#endif // !YAGA_ENGINE_MESH
