#ifndef YAGA_COMMON_LOG
#define YAGA_COMMON_LOG

#include <string>
#include <boost/optional.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/expressions/keyword.hpp>

#include "Exception.h"

#define LOG(severity, msg) { yaga::Log((yaga::log::Severity::severity), (msg), __FILE__, __LINE__); }

namespace yaga { namespace log
{
	typedef boost::log::trivial::severity_level Severity;

	BOOST_LOG_ATTRIBUTE_KEYWORD(a_file, "File", std::string)
	BOOST_LOG_ATTRIBUTE_KEYWORD(a_line, "Line", uint32_t)

	//Severity SeverityFromString(std::string str);

	void Init(boost::optional<Severity> severity = boost::none);
}}

namespace yaga
{
	// --------------------------------------------------------------------------------------------
	inline void Log(log::Severity severity, const std::string& msg, const std::string& file, int line)
	{
		using namespace boost::log;

		BOOST_LOG_STREAM_WITH_PARAMS(trivial::logger::get(), (keywords::severity = severity))
			<< add_value(log::a_file, file) \
			<< add_value(log::a_line, line) \
			<< msg;
	}

	// --------------------------------------------------------------------------------------------
	inline void Log(log::Severity severity, const std::string& msg)
	{
		using namespace boost::log;

		BOOST_LOG_STREAM_WITH_PARAMS(trivial::logger::get(), (keywords::severity = severity)) << msg;
	}

	// --------------------------------------------------------------------------------------------
	inline void Log(const Exception& e)
	{
		BOOST_LOG_TRIVIAL(error)
			<< boost::log::add_value(log::a_file, e.File())
			<< boost::log::add_value(log::a_line, e.Line())
			<< e.what();
	}
}

#endif // !YAGA_COMMON_LOG