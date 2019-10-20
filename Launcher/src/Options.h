#ifndef YAGA_LAUNCHER_OPTIONS
#define YAGA_LAUNCHER_OPTIONS

#include <string>
#include <boost/optional.hpp>

#include "Log.h"

namespace yaga
{
	class Options
	{
	public:
		explicit Options(int argc, char *argv[]);
		const std::string& AppDir() const { return _appDir; }
		boost::optional<log::Severity> LogSeverity() const { return _severity; }
	private:
		std::string _appDir;
		boost::optional<log::Severity> _severity;
	};
}

#endif // !YAGA_LAUNCHER_OPTIONS_PCH
