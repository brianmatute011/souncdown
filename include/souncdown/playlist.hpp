/**
 * @file playlist.hpp
 * @brief SoundCloud playlist detection and parsing
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <expected>
#include <string>
#include <vector>

namespace souncdown {

/**
 * @class Playlist
 * @brief Handle SoundCloud playlist operations
 * 
 * Detects whether a URL is a playlist or single track,
 * parses playlist metadata, and extracts track information.
 */
class Playlist {
public:
    /**
     * @brief Analyze a URL to get playlist/track information
     * @param url SoundCloud URL
     * @param oauth_token Optional OAuth token for private content
     * @return PlaylistInfo on success, error message on failure
     * @throws PlaylistError if JSON parsing fails
     */
    [[nodiscard]] static std::expected<PlaylistInfo, std::string> get_info(
        const std::string& url,
        const std::optional<std::string>& oauth_token = std::nullopt
    );
    
    /**
     * @brief Check if URL points to a playlist (vs single track)
     * @param url SoundCloud URL
     * @return true if playlist, false if single track
     */
    [[nodiscard]] static bool is_playlist_url(const std::string& url);
    
    /**
     * @brief Get available audio formats for a URL
     * @param url SoundCloud URL
     * @param oauth_token Optional OAuth token
     * @return Vector of format strings on success
     */
    [[nodiscard]] static std::expected<std::vector<std::string>, std::string> 
    get_available_formats(
        const std::string& url,
        const std::optional<std::string>& oauth_token = std::nullopt
    );
    
    /**
     * @brief Display playlist information in a formatted way
     * @param info Playlist information to display
     */
    static void display_info(const PlaylistInfo& info);

private:
    /**
     * @brief Parse JSON output from yt-dlp
     * @param json_str JSON string from yt-dlp --dump-json
     * @return PlaylistInfo on success
     */
    [[nodiscard]] static PlaylistInfo parse_json(const std::string& json_str);
    
    /**
     * @brief Build yt-dlp command for info retrieval
     * @param url Target URL
     * @param oauth_token Optional OAuth token
     * @return Command arguments vector
     */
    [[nodiscard]] static std::vector<std::string> build_info_command(
        const std::string& url,
        const std::optional<std::string>& oauth_token
    );
};

} // namespace souncdown
