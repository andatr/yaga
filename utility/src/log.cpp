#include "precompiled.h"
#include "log.h"

namespace yaga {
namespace log {

namespace attrs = boost::log::attributes;
namespace logging = boost::log;
namespace dt = std::chrono;

typedef dt::steady_clock::time_point time_point;

constexpr const char* a_time_str = "a_time";
constexpr const char* a_format_str = "a_format";
BOOST_LOG_ATTRIBUTE_KEYWORD(a_time, a_time_str, time_point)
BOOST_LOG_ATTRIBUTE_KEYWORD(a_format, a_format_str, int)
const auto a_severity = logging::aux::default_attribute_names::severity();

// --------------------------------------------------------------------------------------------
void yagaFormatter(logging::record_view const& rec, logging::formatting_ostream& strm)
{
  const auto& format = rec.attribute_values()[a_format].get();
  const auto& start = rec.attribute_values()[a_time].get();
  const auto& now = dt::steady_clock::now();
  const auto time = static_cast<uint64_t>(dt::duration_cast<dt::milliseconds>(now - start).count());
  const auto& severity = rec.attribute_values()[a_severity].extract<Severity>().get();
  const auto& file = rec.attribute_values()[a_file];
  const auto& line = rec.attribute_values()[a_line];
  if (format & format::time) strm << time << " ";
  if (format & format::severity) strm << severity << " ";
  if (format & format::file) strm << file << " ";
  if (format & format::line) strm << line << " ";
  strm << rec[logging::expressions::smessage];
}

// --------------------------------------------------------------------------------------------
void init(boost::optional<Severity> severity, int format)
{
  auto core = logging::core::get();
  core->add_global_attribute(a_time_str, attrs::constant<time_point>(dt::steady_clock::now()));
  core->add_global_attribute(a_format_str, attrs::constant<int>(format));
  auto sink = logging::add_console_log(std::cout);
  sink->set_formatter(&yagaFormatter);
  if (severity) {
    core->set_filter(logging::trivial::severity >= severity.value());
  }
}

// --------------------------------------------------------------------------------------------
Severity severityFromString(std::string str)
{
  using namespace logging::trivial;

  boost::algorithm::to_lower(str);
#define SEVERITY_FROM_STR(str, severity)                                                                                       \
  if (str == #severity) return severity;
  SEVERITY_FROM_STR(str, trace)
  SEVERITY_FROM_STR(str, debug)
  SEVERITY_FROM_STR(str, info)
  SEVERITY_FROM_STR(str, warning)
  SEVERITY_FROM_STR(str, error)
  SEVERITY_FROM_STR(str, fatal)
#undef SEVERITY_FROM_STR
  THROW("unknown log severity level");
}

} // !namespace log
} // !namespace yaga
