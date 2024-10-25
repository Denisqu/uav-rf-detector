#include "utils/logger.h"


namespace logger {

std::vector<spdlog::sink_ptr>& sinksVector() {

	auto static sinks = std::vector<spdlog::sink_ptr>{
		std::make_shared<spdlog::sinks::rotating_file_sink_mt>("log_filename", 1024 * 1024, 5, false),
		std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
	};

	return sinks;
}

void initThreadPool()
{
	static bool init = []() {
		spdlog::init_thread_pool(8192, 1);
		return true;
	}();
}

}
