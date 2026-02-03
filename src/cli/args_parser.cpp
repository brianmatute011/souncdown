/**
 * @file args_parser.cpp
 * @brief Command-line argument parser implementation
 */

#include "souncdown/args_parser.hpp"
#include <fmt/core.h>
#include <fstream>
#include <sstream>

namespace souncdown {

std::expected<ParsedArgs, std::string> ArgsParser::parse(int argc, char* argv[]) {
    ParsedArgs args;
    
    if (argc < 2) {
        return std::unexpected("No arguments provided");
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.mode = CommandMode::HELP;
            return args;
        }
        else if (arg == "--version" || arg == "-v") {
            args.mode = CommandMode::VERSION;
            return args;
        }
        else if (arg == "--check-deps") {
            args.mode = CommandMode::CHECK_DEPS;
            return args;
        }
        else if (arg == "--info") {
            args.mode = CommandMode::INFO;
        }
        else if (arg == "--show-formats") {
            args.mode = CommandMode::FORMATS;
        }
        else if (arg == "--format" || arg == "-f") {
            if (++i >= argc) return std::unexpected("--format requires an argument");
            auto format_result = parse_format(argv[i]);
            if (!format_result) return std::unexpected(format_result.error());
            args.options.output_format = *format_result;
        }
        else if (arg == "--output" || arg == "-o") {
            if (++i >= argc) return std::unexpected("--output requires an argument");
            args.options.output_directory = argv[i];
        }
        else if (arg == "--oauth") {
            if (++i >= argc) return std::unexpected("--oauth requires an argument");
            args.options.oauth_token = argv[i];
        }
        else if (arg == "--from-file") {
            if (++i >= argc) return std::unexpected("--from-file requires an argument");
            auto urls_result = read_urls_from_file(argv[i]);
            if (!urls_result) return std::unexpected(urls_result.error());
            args.urls.insert(args.urls.end(), urls_result->begin(), urls_result->end());
        }
        else if (arg == "--number") {
            args.options.number_files = true;
        }
        else if (arg == "--max-downloads") {
            if (++i >= argc) return std::unexpected("--max-downloads requires an argument");
            args.options.max_downloads = std::stoul(argv[i]);
        }
        else if (arg == "--playlist-start") {
            if (++i >= argc) return std::unexpected("--playlist-start requires an argument");
            args.options.playlist_start = std::stoul(argv[i]);
        }
        else if (arg == "--no-thumbnail") {
            args.options.embed_thumbnail = false;
        }
        else if (arg == "--no-metadata") {
            args.options.embed_metadata = false;
        }
        else if (arg == "--quality") {
            if (++i >= argc) return std::unexpected("--quality requires an argument");
            args.options.quality = argv[i];
        }
        else if (arg == "--verbose") {
            args.verbose = true;
        }
        else if (arg == "--quiet") {
            args.quiet = true;
        }
        else if (arg == "--progress" || arg == "--interactive") {
            args.show_progress = true;
        }
        else if (!arg.starts_with("-")) {
            // It's a URL
            args.urls.push_back(arg);
        }
        else {
            return std::unexpected(fmt::format("Unknown argument: {}", arg));
        }
    }
    
    return validate(args).transform([&args](bool) { return args; });
}

void ArgsParser::print_help(std::string_view program_name) {
    fmt::print(R"(
souncdown - High-performance SoundCloud downloader

USAGE:
    {} [OPTIONS] [URLs...]

OPTIONS:
    -h, --help                  Show this help message
    -v, --version               Show version information
    --check-deps                Check if dependencies are installed
    
    --info                      Show playlist information without downloading
    --show-formats              Show available formats for URLs
    
    -f, --format FORMAT         Output format (mp3, wav, flac, etc.) [default: mp3]
    -o, --output DIR            Output directory [default: current directory]
    
    --oauth TOKEN               OAuth token for private tracks
    --from-file FILE            Read URLs from file (one per line)
    
    --number                    Number playlist files (01 - Track.mp3, etc.)
    --max-downloads N           Limit downloads to first N tracks
    --playlist-start N          Start downloading from track N [default: 1]
    
    --no-thumbnail              Don't embed thumbnails
    --no-metadata               Don't embed metadata
    --quality QUALITY           Audio quality (bestaudio, best, etc.) [default: bestaudio]
    
    --verbose                   Verbose output
    --quiet                     Minimal output
    --progress, --interactive   Show interactive progress bars (like tqdm)

)", program_name);
}

void ArgsParser::print_version() {
    fmt::print("souncdown version {}\n", SOUNCDOWN_VERSION);
    fmt::print("Built with C++20\n");
}

void ArgsParser::print_examples() {
    fmt::print(R"(
EXAMPLES:
    # Download a single track
    souncdown https://soundcloud.com/artist/track
    
    # Show playlist information
    souncdown --info https://soundcloud.com/artist/sets/playlist
    
    # Download playlist with numbered files
    souncdown --number --output ~/Music https://soundcloud.com/artist/sets/playlist
    
    # Download first 10 tracks only
    souncdown --max-downloads 10 https://soundcloud.com/artist/sets/playlist
    
    # Download tracks 5-15 from playlist
    souncdown --playlist-start 5 --max-downloads 11 URL
    
    # Download as FLAC with OAuth
    souncdown --format flac --oauth TOKEN URL
)");
}

std::expected<bool, std::string> ArgsParser::validate(const ParsedArgs& args) {
    if (args.mode == CommandMode::DOWNLOAD && args.urls.empty()) {
        return std::unexpected("No URLs provided");
    }
    
    if (args.options.max_downloads && *args.options.max_downloads < 1) {
        return std::unexpected("--max-downloads must be at least 1");
    }
    
    if (args.options.playlist_start < 1) {
        return std::unexpected("--playlist-start must be at least 1");
    }
    
    return true;
}

std::expected<AudioFormat, std::string> ArgsParser::parse_format(std::string_view format_str) {
    auto format = audio_format_from_string(format_str);
    if (!format) {
        return std::unexpected(fmt::format("Invalid format: {}", format_str));
    }
    return *format;
}

std::expected<std::vector<std::string>, std::string> 
ArgsParser::read_urls_from_file(const fs::path& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return std::unexpected(fmt::format("Cannot open file: {}", file_path.string()));
    }
    
    std::vector<std::string> urls;
    std::string line;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Skip empty lines and comments
        if (!line.empty() && !line.starts_with("#")) {
            urls.push_back(line);
        }
    }
    
    return urls;
}

} // namespace souncdown
