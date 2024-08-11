#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <memory>
#include <vector>

namespace logger {

std::vector<spdlog::sink_ptr>& sinksVector();
void initThreadPool();

}

#define LOG_CAT(name, ...) \
    spdlog::logger* name() \
    {                      \
        logger::initThreadPool(); \
        auto& sinks = logger::sinksVector(); \
        static auto threadPool = spdlog::thread_pool(); \
        static auto logger = std::make_shared<spdlog::async_logger>(__VA_ARGS__, sinks.begin(), sinks.end(), threadPool, spdlog::async_overflow_policy::block); \
        return logger.get(); \
    }

#ifdef _DEBUG
#define LOG_DEBUG(category, ...)    SPDLOG_LOGGER_DEBUG(category(), __VA_ARGS__);
#define LOG_INFO(category, ...)     SPDLOG_LOGGER_INFO(category(), __VA_ARGS__);
#define LOG_WARNING(category, ...)  SPDLOG_LOGGER_WARN(category(), __VA_ARGS__);
#define LOG_CRITICAL(category, ...) SPDLOG_LOGGER_CRITICAL(category(), __VA_ARGS__);
#define LOG_FATAL(category, ...)    SPDLOG_LOGGER_ERROR(category(), __VA_ARGS__);
#else
#define LOG_DEBUG(category, ...)    category()->debug(__VA_ARGS__);
#define LOG_INFO(category, ...)     category()->info(__VA_ARGS__);
#define LOG_WARNING(category, ...)  category()->warning(__VA_ARGS__);
#define LOG_CRITICAL(category, ...) category()->critical(__VA_ARGS__);
#define LOG_FATAL(category, ...)    category()->fatal(__VA_ARGS__);
#endif
#define log_debug(category, ...)    LOG_DEBUG(category, __VA_ARGS__)
#define log_info(category, ...)     LOG_INFO(category, __VA_ARGS__)
#define log_warning(category, ...)  LOG_WARNING(category, __VA_ARGS__)
#define log_critical(category, ...) LOG_CRITICAL(category, __VA_ARGS__)
#define log_fatal(category, ...)    LOG_FATAL(category, __VA_ARGS__)

#endif //LOGGER_H
