#ifndef YAGA_ENGINE_TEXTURE
#define YAGA_ENGINE_TEXTURE

#include <boost/noncopyable.hpp>
#include <GLFW/glfw3.h>

#include "AutoDeleter.h"

namespace yaga
{
	class Texture : private boost::noncopyable
	{
	public:
		Texture(VkDevice device, VkImage image, VkFormat format);
		VkImageView ImageView() const { return *_imageView; }
	private:
		AutoDeleter<VkImageView> _imageView;
	};
}

#endif // !YAGA_ENGINE_TEXTURE
