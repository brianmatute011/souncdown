/**
 * @file config.hpp
 * @brief Configuration file handling
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <expected>
#include <string>

namespace souncdown {

/**
 * @class Config
 * @brief Handle application configuration
 * 
 * Manages persistent configuration stored in user's home directory.
 * Format: TOML or JSON for easy editing.
 */
class Config {
public:
    /**
     * @brief Load configuration from default location
     * @return Expected with DownloadOptions on success
     */
    [[nodiscard]] static std::expected<DownloadOptions, std::string> load();
    
    /**
     * @brief Load configuration from specific file
     * @param config_path Path to configuration file
     * @return Expected with DownloadOptions on success
     */
    [[nodiscard]] static std::expected<DownloadOptions, std::string> load_from(
        const fs::path& config_path
    );
    
    /**
     * @brief Save configuration to default location
     * @param options Options to save
     * @return Expected with true on success
     */
    [[nodiscard]] static std::expected<bool, std::string> save(
        const DownloadOptions& options
    );
    
    /**
     * @brief Save configuration to specific file
     * @param options Options to save
     * @param config_path Path to configuration file
     * @return Expected with true on success
     */
    [[nodiscard]] static std::expected<bool, std::string> save_to(
        const DownloadOptions& options,
        const fs::path& config_path
    );
    
    /**
     * @brief Get default configuration file path
     * @return Path to default config location (~/.config/souncdown/config.json)
     */
    [[nodiscard]] static fs::path default_config_path();
    
    /**
     * @brief Check if configuration file exists
     * @return true if config file exists
     */
    [[nodiscard]] static bool exists();
    
    /**
     * @brief Create default configuration file
     * @return Expected with true on success
     */
    [[nodiscard]] static std::expected<bool, std::string> create_default();

private:
    /**
     * @brief Parse JSON configuration
     * @param json_str JSON string
     * @return DownloadOptions parsed from JSON
     */
    [[nodiscard]] static DownloadOptions parse_json(const std::string& json_str);
    
    /**
     * @brief Convert options to JSON string
     * @param options Options to convert
     * @return JSON string representation
     */
    [[nodiscard]] static std::string to_json(const DownloadOptions& options);
};

} // namespace souncdown
