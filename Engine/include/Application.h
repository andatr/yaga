#ifndef YAGA_ENGINE_APPLICATION
#define YAGA_ENGINE_APPLICATION

#include <string>
#include <memory>
#include <boost/core/noncopyable.hpp>

namespace yaga
{
	class Application : private boost::noncopyable
	{
	public:
		virtual ~Application() {}
		virtual void Run(const std::string& dir) = 0;
	};

	typedef std::unique_ptr<Application> ApplicationPtr;

	ApplicationPtr CreateApplication();
}

#endif // !YAGA_ENGINE_APPLICATION
