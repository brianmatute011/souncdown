/**
 * @file logger.cpp
 * @brief Logger implementation
 */

#include "souncdown/logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>

namespace souncdown {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    try {
        // Create console sink with colors
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        
        // Create rotating file sink
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "/tmp/souncdown.log", true
        );
        file_sink->set_level(spdlog::level::trace);
        
        // Combine sinks
        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        logger_ = std::make_shared<spdlog::logger>(
            "souncdown", sinks.begin(), sinks.end()
        );
        
        // Set pattern: [timestamp] [level] message
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        logger_->set_level(spdlog::level::info);
        
        // Register as default logger
        spdlog::set_default_logger(logger_);
        
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
}

void Logger::set_level(LogLevel level) {
    spdlog::level::level_enum spdlog_level;
    
    switch (level) {
        case LogLevel::TRACE:    spdlog_level = spdlog::level::trace; break;
        case LogLevel::DEBUG:    spdlog_level = spdlog::level::debug; break;
        case LogLevel::INFO:     spdlog_level = spdlog::level::info; break;
        case LogLevel::WARN:     spdlog_level = spdlog::level::warn; break;
        case LogLevel::ERROR:    spdlog_level = spdlog::level::err; break;
        case LogLevel::CRITICAL: spdlog_level = spdlog::level::critical; break;
        default:                 spdlog_level = spdlog::level::info;
    }
    
    logger_->set_level(spdlog_level);
}

} // namespace souncdown
