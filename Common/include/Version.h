#ifndef YAGA_COMMON_VERSION
#define YAGA_COMMON_VERSION

#include <stdint.h>

namespace yaga
{
	struct Version
	{
		uint16_t major;
		uint16_t minor;
		uint32_t build;

		explicit Version(uint16_t major = 0, uint16_t minor = 0, uint32_t build = 0): 
			major(major), minor(minor), build(build)
		{
		}
	};
}

#endif // !YAGA_COMMON_VERSION
