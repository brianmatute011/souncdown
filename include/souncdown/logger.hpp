/**
 * @file logger.hpp
 * @brief Modern logging facility using spdlog
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <spdlog/spdlog.h>
#include <memory>
#include <string_view>

namespace souncdown {

/**
 * @class Logger
 * @brief Thread-safe logging wrapper around spdlog
 * 
 * Provides a convenient interface for logging with different levels
 * and automatic formatting. Singleton pattern for global access.
 */
class Logger {
public:
    /**
     * @brief Get the singleton logger instance
     * @return Reference to the global logger
     */
    static Logger& instance();
    
    /**
     * @brief Set the logging level
     * @param level Minimum level to log
     */
    void set_level(LogLevel level);
    
    /**
     * @brief Log a trace message
     * @tparam Args Variadic template for fmt-style arguments
     */
    template<typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->trace(fmt, std::forward<Args>(args)...);
    }
    
    /**
     * @brief Log a debug message
     */
    template<typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->debug(fmt, std::forward<Args>(args)...);
    }
    
    /**
     * @brief Log an info message
     */
    template<typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->info(fmt, std::forward<Args>(args)...);
    }
    
    /**
     * @brief Log a warning message
     */
    template<typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->warn(fmt, std::forward<Args>(args)...);
    }
    
    /**
     * @brief Log an error message
     */
    template<typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->error(fmt, std::forward<Args>(args)...);
    }
    
    /**
     * @brief Log a critical message
     */
    template<typename... Args>
    void critical(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->critical(fmt, std::forward<Args>(args)...);
    }

    // Delete copy/move constructors and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    Logger();
    ~Logger() = default;
    
    std::shared_ptr<spdlog::logger> logger_;
};

// Convenience macros for global logging
#define LOG_TRACE(...)    souncdown::Logger::instance().trace(__VA_ARGS__)
#define LOG_DEBUG(...)    souncdown::Logger::instance().debug(__VA_ARGS__)
#define LOG_INFO(...)     souncdown::Logger::instance().info(__VA_ARGS__)
#define LOG_WARN(...)     souncdown::Logger::instance().warn(__VA_ARGS__)
#define LOG_ERROR(...)    souncdown::Logger::instance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) souncdown::Logger::instance().critical(__VA_ARGS__)

} // namespace souncdown
