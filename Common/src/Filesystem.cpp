#include "Pch.h"
#include "Filesystem.h"
#include "Exception.h"

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	ByteArray ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file) {
			THROW("Could not open file %1%", filename);
		}
		ByteArray content(static_cast<size_t>(file.tellg())); // works with empty files too
		file.seekg(0);
		file.read(*content, content.Size());
		return content;
	}
}

