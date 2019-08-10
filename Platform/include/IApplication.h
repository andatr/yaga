#ifndef YAGA_PLATFORM_IAPPLICATION
#define YAGA_PLATFORM_IAPPLICATION

#include <string>
#include <memory>
#include <boost/core/noncopyable.hpp>

namespace yaga
{
	class IApplication : private boost::noncopyable
	{
	public:
		virtual ~IApplication() {}
		virtual void Run() = 0;
	};

	typedef std::unique_ptr<IApplication> IApplicationPtr;

	IApplicationPtr CreateApplication(int width, int height, const std::string& title);
}

#endif // !YAGA_PLATFORM_IAPPLICATION