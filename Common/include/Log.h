#ifndef YAGA_COMMON_LOG
#define YAGA_COMMON_LOG

#include <string>
#include <boost/optional.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/expressions/keyword.hpp>

#include "Exception.h"

#define LOG(severity) BOOST_LOG_TRIVIAL(severity) \
		<< boost::log::add_value(yaga::log::a_file, __FILE__) \
		<< boost::log::add_value(yaga::log::a_line, __LINE__)

#define LOG_E(severity, exception) BOOST_LOG_TRIVIAL(severity) \
		<< boost::log::add_value(yaga::log::a_file, exception.File()) \
		<< boost::log::add_value(yaga::log::a_line, exception.Line()) \
		<< exception.what()

namespace yaga { namespace log
{
	typedef boost::log::trivial::severity_level Severity;

	BOOST_LOG_ATTRIBUTE_KEYWORD(a_file, "File", std::string)
		BOOST_LOG_ATTRIBUTE_KEYWORD(a_line, "Line", uint32_t)

		enum class Format : int
	{
		Time,
		Severity,
		File,
		Line,
		All = std::numeric_limits<int>::max()
	};

	void Init(boost::optional<Severity> severity = boost::none, Format format = Format::All);
	Severity SeverityFromString(std::string str);
}}

#endif // !YAGA_COMMON_LOG