/**
 * @file config.cpp
 * @brief Configuration file handling implementation
 */

#include "souncdown/config.hpp"
#include "souncdown/logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdlib>

using json = nlohmann::json;

namespace souncdown {

fs::path Config::default_config_path() {
    const char* home = std::getenv("HOME");
    if (!home) {
        home = std::getenv("USERPROFILE");  // Windows fallback
    }
    
    if (!home) {
        return ".config/souncdown/config.json";
    }
    
    return fs::path(home) / ".config" / "souncdown" / "config.json";
}

bool Config::exists() {
    return fs::exists(default_config_path());
}

std::expected<DownloadOptions, std::string> Config::load() {
    return load_from(default_config_path());
}

std::expected<DownloadOptions, std::string> Config::load_from(
    const fs::path& config_path
) {
    if (!fs::exists(config_path)) {
        return std::unexpected("Config file does not exist");
    }
    
    try {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            return std::unexpected("Cannot open config file");
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        
        return parse_json(content);
        
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Failed to load config: ") + e.what());
    }
}

std::expected<bool, std::string> Config::save(const DownloadOptions& options) {
    return save_to(options, default_config_path());
}

std::expected<bool, std::string> Config::save_to(
    const DownloadOptions& options,
    const fs::path& config_path
) {
    try {
        // Create directory if it doesn't exist
        fs::create_directories(config_path.parent_path());
        
        std::ofstream file(config_path);
        if (!file.is_open()) {
            return std::unexpected("Cannot create config file");
        }
        
        file << to_json(options);
        
        return true;
        
    } catch (const std::exception& e) {
        return std::unexpected(std::string("Failed to save config: ") + e.what());
    }
}

std::expected<bool, std::string> Config::create_default() {
    DownloadOptions default_options;
    return save(default_options);
}

DownloadOptions Config::parse_json(const std::string& json_str) {
    auto j = json::parse(json_str);
    
    DownloadOptions options;
    
    if (j.contains("output_directory")) {
        options.output_directory = j["output_directory"].get<std::string>();
    }
    
    if (j.contains("output_format")) {
        auto format = audio_format_from_string(j["output_format"].get<std::string>());
        if (format) {
            options.output_format = *format;
        }
    }
    
    if (j.contains("embed_thumbnail")) {
        options.embed_thumbnail = j["embed_thumbnail"].get<bool>();
    }
    
    if (j.contains("embed_metadata")) {
        options.embed_metadata = j["embed_metadata"].get<bool>();
    }
    
    if (j.contains("number_files")) {
        options.number_files = j["number_files"].get<bool>();
    }
    
    if (j.contains("quality")) {
        options.quality = j["quality"].get<std::string>();
    }
    
    return options;
}

std::string Config::to_json(const DownloadOptions& options) {
    json j;
    
    j["output_directory"] = options.output_directory.string();
    j["output_format"] = std::string(to_string(options.output_format));
    j["embed_thumbnail"] = options.embed_thumbnail;
    j["embed_metadata"] = options.embed_metadata;
    j["number_files"] = options.number_files;
    j["quality"] = options.quality;
    
    return j.dump(4);  // Pretty print with 4-space indent
}

} // namespace souncdown
