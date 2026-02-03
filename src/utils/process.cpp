/**
 * @file process.cpp
 * @brief Safe process execution implementation
 */

#include "souncdown/process.hpp"
#include "souncdown/logger.hpp"
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

namespace souncdown {

ProcessResult Process::execute(
    const std::string& command,
    const std::vector<std::string>& args,
    const std::optional<fs::path>& working_dir
) {
    LOG_DEBUG("Executing: {} with {} args", command, args.size());
    
    ProcessResult result;
    
    // Build full command for logging
    std::string full_cmd = command;
    for (const auto& arg : args) {
        full_cmd += " " + arg;
    }
    LOG_TRACE("Full command: {}", full_cmd);
    
    // Create pipes for stdout and stderr
    int stdout_pipe[2];
    int stderr_pipe[2];
    
    if (pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0) {
        throw std::runtime_error("Failed to create pipes");
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        throw std::runtime_error("Failed to fork process");
    }
    
    if (pid == 0) {
        // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        
        // Redirect stdout and stderr
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        
        // Change working directory if specified
        if (working_dir) {
            if (chdir(working_dir->c_str()) != 0) {
                perror("chdir failed");
                _exit(127);
            }
        }
        
        // Build argv for execvp
        std::vector<char*> argv_ptrs;
        argv_ptrs.push_back(const_cast<char*>(command.c_str()));
        for (const auto& arg : args) {
            argv_ptrs.push_back(const_cast<char*>(arg.c_str()));
        }
        argv_ptrs.push_back(nullptr);
        
        execvp(command.c_str(), argv_ptrs.data());
        
        // If exec failed
        perror("execvp failed");
        _exit(127);
    }
    
    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);
    
    // Read stdout
    std::array<char, 128> buffer;
    FILE* stdout_file = fdopen(stdout_pipe[0], "r");
    while (fgets(buffer.data(), buffer.size(), stdout_file) != nullptr) {
        result.stdout_output += buffer.data();
    }
    fclose(stdout_file);
    
    // Read stderr
    FILE* stderr_file = fdopen(stderr_pipe[0], "r");
    while (fgets(buffer.data(), buffer.size(), stderr_file) != nullptr) {
        result.stderr_output += buffer.data();
    }
    fclose(stderr_file);
    
    // Wait for child
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
    } else {
        result.exit_code = -1;
    }
    
    LOG_DEBUG("Process exited with code: {}", result.exit_code);
    
    return result;
}

ProcessResult Process::execute_with_callback(
    const std::string& command,
    const std::vector<std::string>& args,
    OutputCallback callback,
    const std::optional<fs::path>& working_dir
) {
    LOG_DEBUG("Executing with callback: {} with {} args", command, args.size());
    
    ProcessResult result;
    
    // Create pipes
    int stdout_pipe[2];
    int stderr_pipe[2];
    
    if (pipe(stdout_pipe) != 0 || pipe(stderr_pipe) != 0) {
        throw std::runtime_error("Failed to create pipes");
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        throw std::runtime_error("Failed to fork process");
    }
    
    if (pid == 0) {
        // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);
        
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);
        
        if (working_dir) {
            if (chdir(working_dir->c_str()) != 0) {
                perror("chdir failed");
                _exit(127);
            }
        }
        
        std::vector<char*> argv_ptrs;
        argv_ptrs.push_back(const_cast<char*>(command.c_str()));
        for (const auto& arg : args) {
            argv_ptrs.push_back(const_cast<char*>(arg.c_str()));
        }
        argv_ptrs.push_back(nullptr);
        
        execvp(command.c_str(), argv_ptrs.data());
        perror("execvp failed");
        _exit(127);
    }
    
    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);
    
    // Set pipes to non-blocking for real-time reading
    fcntl(stdout_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(stderr_pipe[0], F_SETFL, O_NONBLOCK);
    
    std::string stdout_buffer;
    std::string stderr_buffer;
    std::array<char, 4096> buffer;
    
    bool stdout_open = true;
    bool stderr_open = true;
    
    // Read from both pipes until both are closed
    while (stdout_open || stderr_open) {
        // Try reading from stdout
        if (stdout_open) {
            ssize_t n = read(stdout_pipe[0], buffer.data(), buffer.size());
            if (n > 0) {
                std::string chunk(buffer.data(), n);
                stdout_buffer += chunk;
                result.stdout_output += chunk;
                
                // Process complete lines
                std::size_t pos;
                while ((pos = stdout_buffer.find('\n')) != std::string::npos) {
                    std::string line = stdout_buffer.substr(0, pos);
                    stdout_buffer.erase(0, pos + 1);
                    if (callback) {
                        callback(line, false);
                    }
                }
            } else if (n == 0) {
                stdout_open = false;
            }
        }
        
        // Try reading from stderr
        if (stderr_open) {
            ssize_t n = read(stderr_pipe[0], buffer.data(), buffer.size());
            if (n > 0) {
                std::string chunk(buffer.data(), n);
                stderr_buffer += chunk;
                result.stderr_output += chunk;
                
                std::size_t pos;
                while ((pos = stderr_buffer.find('\n')) != std::string::npos) {
                    std::string line = stderr_buffer.substr(0, pos);
                    stderr_buffer.erase(0, pos + 1);
                    if (callback) {
                        callback(line, true);
                    }
                }
            } else if (n == 0) {
                stderr_open = false;
            }
        }
        
        // Small sleep to avoid busy waiting
        if (stdout_open || stderr_open) {
            usleep(10000); // 10ms
        }
    }
    
    // Process any remaining content
    if (!stdout_buffer.empty() && callback) {
        callback(stdout_buffer, false);
    }
    if (!stderr_buffer.empty() && callback) {
        callback(stderr_buffer, true);
    }
    
    close(stdout_pipe[0]);
    close(stderr_pipe[0]);
    
    // Wait for child
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        result.exit_code = WEXITSTATUS(status);
    } else {
        result.exit_code = -1;
    }
    
    LOG_DEBUG("Process exited with code: {}", result.exit_code);
    
    return result;
}

bool Process::executable_exists(const std::string& executable_name) {
    return find_executable(executable_name).has_value();
}

std::optional<fs::path> Process::find_executable(const std::string& executable_name) {
    // Check in PATH
    const char* path_env = std::getenv("PATH");
    if (!path_env) {
        return std::nullopt;
    }
    
    std::string path_str(path_env);
    std::istringstream path_stream(path_str);
    std::string path_entry;
    
    while (std::getline(path_stream, path_entry, ':')) {
        fs::path full_path = fs::path(path_entry) / executable_name;
        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
            // Check if executable
            if (access(full_path.c_str(), X_OK) == 0) {
                return full_path;
            }
        }
    }
    
    return std::nullopt;
}

std::expected<bool, std::string> Process::check_dependencies() {
    std::vector<std::string> missing;
    
    if (!executable_exists("yt-dlp")) {
        missing.push_back("yt-dlp");
    }
    
    if (!executable_exists("ffmpeg")) {
        missing.push_back("ffmpeg");
    }
    
    if (!missing.empty()) {
        std::string error = "Missing required dependencies: ";
        for (size_t i = 0; i < missing.size(); ++i) {
            if (i > 0) error += ", ";
            error += missing[i];
        }
        error += "\n\nInstallation instructions:\n";
        error += "  - yt-dlp: pip install yt-dlp\n";
        error += "  - ffmpeg: sudo apt install ffmpeg (Linux) or brew install ffmpeg (macOS)";
        return std::unexpected(error);
    }
    
    return true;
}

std::string Process::escape_argument(const std::string& arg) {
    // Arguments are passed to execvp, not shell, so no escaping needed
    return arg;
}

std::vector<std::string> Process::build_command_line(
    const std::string& command,
    const std::vector<std::string>& args
) {
    std::vector<std::string> cmd_line;
    cmd_line.push_back(command);
    cmd_line.insert(cmd_line.end(), args.begin(), args.end());
    return cmd_line;
}

} // namespace souncdown
