/**
 * @file args_parser.hpp
 * @brief Command-line argument parsing
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <string>
#include <vector>
#include <optional>
#include <expected>

namespace souncdown {

/**
 * @enum CommandMode
 * @brief Application operation mode
 */
enum class CommandMode {
    DOWNLOAD,       ///< Download tracks/playlists
    INFO,           ///< Show playlist information
    FORMATS,        ///< Show available formats
    CHECK_DEPS,     ///< Check dependencies
    VERSION,        ///< Show version
    HELP            ///< Show help
};

/**
 * @struct ParsedArgs
 * @brief Parsed command-line arguments
 */
struct ParsedArgs {
    CommandMode mode{CommandMode::DOWNLOAD};
    std::vector<std::string> urls;
    DownloadOptions options;
    bool verbose{false};
    bool quiet{false};
    bool show_progress{false};  ///< Show interactive progress bars
};

/**
 * @class ArgsParser
 * @brief Modern command-line argument parser
 * 
 * Provides user-friendly CLI interface with help messages,
 * validation, and sensible defaults.
 */
class ArgsParser {
public:
    /**
     * @brief Parse command-line arguments
     * @param argc Argument count
     * @param argv Argument values
     * @return Expected with ParsedArgs on success, error message on failure
     */
    [[nodiscard]] static std::expected<ParsedArgs, std::string> parse(
        int argc, 
        char* argv[]
    );
    
    /**
     * @brief Display help message
     * @param program_name Name of the program executable
     */
    static void print_help(std::string_view program_name);
    
    /**
     * @brief Display version information
     */
    static void print_version();
    
    /**
     * @brief Display usage examples
     */
    static void print_examples();

private:
    /**
     * @brief Validate parsed arguments
     * @param args Arguments to validate
     * @return Expected with true on success, error message on failure
     */
    [[nodiscard]] static std::expected<bool, std::string> validate(
        const ParsedArgs& args
    );
    
    /**
     * @brief Parse format string to AudioFormat enum
     * @param format_str Format string (e.g., "mp3", "wav")
     * @return Expected with AudioFormat on success
     */
    [[nodiscard]] static std::expected<AudioFormat, std::string> parse_format(
        std::string_view format_str
    );
    
    /**
     * @brief Read URLs from file
     * @param file_path Path to file with URLs (one per line)
     * @return Expected with vector of URLs on success
     */
    [[nodiscard]] static std::expected<std::vector<std::string>, std::string> 
    read_urls_from_file(const fs::path& file_path);
};

} // namespace souncdown
