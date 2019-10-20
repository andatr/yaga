#ifndef YAGA_COMMON_EXCEPTION
#define YAGA_COMMON_EXCEPTION

#include <exception>
#include <boost/format.hpp>

#define THROW(format, ...) throw yaga::Exception(__FILE__, __LINE__, format, __VA_ARGS__)

namespace yaga
{
	// -------------------------------------------------------------------------------------------------------------------------
	class Exception : public std::exception
	{
	public:
		template<typename ...Args>
		explicit Exception(const std::string& file, int line, const std::string& format, Args... args);
		const std::string& File() const { return _file; }
		int Line() const { return _line; }
		const char* what() const override { return _message.c_str(); }
	private:
		const std::string _file;
		int _line;
		const std::string _message;
	};

	// -------------------------------------------------------------------------------------------------------------------------
	inline std::string ExpandFormat(boost::format f)
	{
		return f.str();
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename T, typename... Args>
	std::string ExpandFormat(boost::format f, T&& t, Args&&... args)
	{
		return ExpandFormat(f % std::forward<T>(t), std::forward<Args>(args)...);
	}

	// -------------------------------------------------------------------------------------------------------------------------
	template<typename ...Args>
	Exception::Exception(const std::string& file, int line, const std::string& format, Args... args):
		_file(file), _line(line), _message(ExpandFormat(boost::format(format), args...))
	{
	}
}

#endif // !YAGA_COMMON_EXCEPTION