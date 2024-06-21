#include "lib/utils/logger.h"

namespace logger {

std::vector<spdlog::sink_ptr>& sinksVector() {

	auto static sinks = std::vector<spdlog::sink_ptr>{
		std::make_shared<spdlog::sinks::rotating_file_sink_mt>("log_filename", 1024 * 1024, 5, false),
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
	};

	for (const auto& sink : sinks) {
		sink->set_pattern("[source %s] [function %!]:[%#] %v");
	}

	return sinks;
}

}
