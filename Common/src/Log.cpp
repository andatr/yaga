#include "Pch.h"
#include "include/Log.h"

namespace yaga { namespace log
{
	namespace logging = boost::log;
	namespace attrs = logging::attributes;
	namespace sinks = boost::log::sinks;
	namespace dt = std::chrono;

	typedef dt::steady_clock::time_point time_point;

	constexpr const char* a_time_str = "a_time";
	BOOST_LOG_ATTRIBUTE_KEYWORD(a_time, a_time_str, time_point)
	const auto a_severity = logging::aux::default_attribute_names::severity();

	// --------------------------------------------------------------------------------------------
	class SinkConsole : public sinks::basic_formatted_sink_backend<char, sinks::synchronized_feeding>
	{
	public:
		static void consume(boost::log::record_view const& rec, string_type const& command_line);
	};
	
	// --------------------------------------------------------------------------------------------
	void SinkConsole::consume(logging::record_view const& rec, string_type const& message)
	{
		const auto& now = dt::steady_clock::now();
		const auto& start = rec.attribute_values()[a_time].get();
		const auto time = static_cast<uint64_t>(dt::duration_cast<dt::milliseconds>(now - start).count());
		
		const auto& severity = rec.attribute_values()[a_severity].extract<Severity>().get();
		const auto& file = rec.attribute_values()[a_file];
		const auto& line = rec.attribute_values()[a_line];

		std::cout
			<< time << " "
			<< severity << " "
			<< file << " "
			<< line << " "
			<< message
		<< std::endl;
	}

	// --------------------------------------------------------------------------------------------
	void Init(boost::optional<Severity> severity)
	{
		typedef sinks::synchronous_sink<SinkConsole> sink_t;

		auto core = logging::core::get();
		if (severity) {
			core->set_filter(logging::trivial::severity >= severity.value());
		}
		core->add_global_attribute(a_time_str, attrs::constant<time_point>(dt::steady_clock::now()));
		auto sink = boost::make_shared<sink_t>();
		core->add_sink(sink);
	}

	// --------------------------------------------------------------------------------------------
	/*Severity SeverityFromString(std::string str)
	{
		using namespace logging::trivial;

		boost::algorithm::to_lower(str);
#define SEVERITY_FROM_STR(str, severity) if (str == #severity) return severity; 
			SEVERITY_FROM_STR(str, trace)
			SEVERITY_FROM_STR(str, debug)
			SEVERITY_FROM_STR(str, info)
			SEVERITY_FROM_STR(str, warning)
			SEVERITY_FROM_STR(str, error)
			SEVERITY_FROM_STR(str, fatal)
#undef SEVERITY_FROM_STR
		THROW("unknown severity level")
	}*/
}}