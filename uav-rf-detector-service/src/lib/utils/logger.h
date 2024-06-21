#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>
#include <vector>

namespace logger {

std::vector<spdlog::sink_ptr>& sinksVector();

}

#define LOG_CAT(name, ...) \
    spdlog::logger& name() \
    { \
        static spdlog::logger category(__VA_ARGS__, logger::sinksVector().begin(), logger::sinksVector().end()); \
        return category; \
    }

#define log_debug(category, ...)    category().debug(__VA_ARGS__);
#define log_info(category, ...)     category().info(__VA_ARGS__);
#define log_warning(category, ...)  category().warning(__VA_ARGS__);
#define log_critical(category, ...) category().critical(__VA_ARGS__);
#define log_fatal(category, ...)    category().fatal(__VA_ARGS__);

#endif //LOGGER_H
