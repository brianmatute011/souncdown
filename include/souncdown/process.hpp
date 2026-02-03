/**
 * @file process.hpp
 * @brief Safe process execution utilities
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <optional>
#include <expected>
#include <functional>

namespace souncdown {

/**
 * @struct ProcessResult
 * @brief Result of a process execution
 */
struct ProcessResult {
    int exit_code{0};
    std::string stdout_output;
    std::string stderr_output;
    
    /**
     * @brief Check if process completed successfully
     */
    [[nodiscard]] bool success() const noexcept {
        return exit_code == 0;
    }
};

/**
 * @class Process
 * @brief Safe wrapper for executing external processes
 * 
 * Prevents command injection vulnerabilities by properly handling
 * arguments and provides async execution with progress callbacks.
 */
class Process {
public:
    /**
     * @brief Callback for real-time output processing
     * @param line Output line from stdout/stderr
     * @param is_stderr True if line is from stderr
     */
    using OutputCallback = std::function<void(std::string_view line, bool is_stderr)>;
    
    /**
     * @brief Execute a command synchronously
     * @param command Executable name or path
     * @param args Command arguments (safe, no shell injection)
     * @param working_dir Optional working directory
     * @return ProcessResult with exit code and output
     * @throws std::runtime_error if process cannot be started
     */
    static ProcessResult execute(
        const std::string& command,
        const std::vector<std::string>& args = {},
        const std::optional<fs::path>& working_dir = std::nullopt
    );
    
    /**
     * @brief Execute a command with real-time output callback
     * @param command Executable name or path
     * @param args Command arguments
     * @param callback Function called for each output line
     * @param working_dir Optional working directory
     * @return ProcessResult with exit code
     * @throws std::runtime_error if process cannot be started
     */
    static ProcessResult execute_with_callback(
        const std::string& command,
        const std::vector<std::string>& args,
        OutputCallback callback,
        const std::optional<fs::path>& working_dir = std::nullopt
    );
    
    /**
     * @brief Check if an executable exists in PATH
     * @param executable_name Name of the executable
     * @return true if found, false otherwise
     */
    [[nodiscard]] static bool executable_exists(const std::string& executable_name);
    
    /**
     * @brief Get the full path of an executable
     * @param executable_name Name of the executable
     * @return Optional path if found
     */
    [[nodiscard]] static std::optional<fs::path> find_executable(
        const std::string& executable_name
    );
    
    /**
     * @brief Verify all required dependencies are installed
     * @return Expected with true if all found, error message otherwise
     */
    [[nodiscard]] static std::expected<bool, std::string> check_dependencies();

private:
    /**
     * @brief Escape argument for safe shell usage (if needed)
     * @param arg Argument to escape
     * @return Escaped argument
     */
    static std::string escape_argument(const std::string& arg);
    
    /**
     * @brief Build command line from command and args
     * @param command Executable
     * @param args Arguments
     * @return Vector of strings for execvp
     */
    static std::vector<std::string> build_command_line(
        const std::string& command,
        const std::vector<std::string>& args
    );
};

} // namespace souncdown
