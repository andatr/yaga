#ifndef YAGA_UTILITY_LOG
#define YAGA_UTILITY_LOG

#include "utility/compiler.h"

#include <string>

DISABLE_WARNINGS
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/optional.hpp>
ENABLE_WARNINGS

#define LOG(severity) BOOST_LOG_TRIVIAL(severity)                 \
  << boost::log::add_value(yaga::log::a_file, __FILE__)           \
  << boost::log::add_value(yaga::log::a_line, __LINE__)

namespace yaga {
namespace log {

typedef boost::log::trivial::severity_level Severity;

BOOST_LOG_ATTRIBUTE_KEYWORD(a_file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(a_line, "Line", uint32_t)

namespace format {

enum Attributes : int
{
  time     = 1,
  severity = 2,
  file     = 4,
  line     = 8,
  all      = std::numeric_limits<int>::max()
};

} // !namespace format

void init(boost::optional<Severity> severity = boost::none, int format = format::all);
Severity severityFromString(std::string str);

} // !namespace log
} // !namespace yaga

#endif // !YAGA_UTILITY_LOG