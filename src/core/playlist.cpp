/**
 * @file playlist.cpp
 * @brief Playlist operations implementation
 */

#include "souncdown/playlist.hpp"
#include "souncdown/process.hpp"
#include "souncdown/logger.hpp"
#include <nlohmann/json.hpp>
#include <fmt/core.h>

using json = nlohmann::json;

namespace souncdown {

std::expected<PlaylistInfo, std::string> Playlist::get_info(
    const std::string& url,
    const std::optional<std::string>& oauth_token
) {
    try {
        auto cmd_args = build_info_command(url, oauth_token);
        auto result = Process::execute("yt-dlp", cmd_args);
        
        if (!result.success()) {
            return std::unexpected(fmt::format(
                "Failed to get playlist info: {}", result.stderr_output
            ));
        }
        
        return parse_json(result.stdout_output);
        
    } catch (const std::exception& e) {
        return std::unexpected(fmt::format("Exception: {}", e.what()));
    }
}

bool Playlist::is_playlist_url(const std::string& url) {
    return url.find("/sets/") != std::string::npos;
}

std::expected<std::vector<std::string>, std::string> 
Playlist::get_available_formats(
    const std::string& url,
    const std::optional<std::string>& oauth_token
) {
    std::vector<std::string> cmd_args = {"-F", url};
    
    if (oauth_token) {
        cmd_args.push_back("--add-header");
        cmd_args.push_back(fmt::format("Authorization: OAuth {}", *oauth_token));
    }
    
    auto result = Process::execute("yt-dlp", cmd_args);
    
    if (!result.success()) {
        return std::unexpected(result.stderr_output);
    }
    
    // Parse output lines
    std::vector<std::string> formats;
    std::istringstream stream(result.stdout_output);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (!line.empty() && !line.starts_with("[info]")) {
            formats.push_back(line);
        }
    }
    
    return formats;
}

void Playlist::display_info(const PlaylistInfo& info) {
    fmt::print("\n{}\n", std::string(80, '='));
    
    if (info.is_playlist()) {
        fmt::print("Type: Playlist\n");
        fmt::print("Title: {}\n", info.title);
        fmt::print("Uploader: {}\n", info.uploader);
        fmt::print("Tracks: {}\n", info.track_count);
    } else {
        fmt::print("Type: Single Track\n");
        fmt::print("Title: {}\n", info.title);
        fmt::print("Uploader: {}\n", info.uploader);
    }
    
    fmt::print("URL: {}\n", info.url);
    fmt::print("{}\n\n", std::string(80, '='));
}

PlaylistInfo Playlist::parse_json(const std::string& json_str) {
    auto j = json::parse(json_str);
    
    PlaylistInfo info;
    info.title = j.value("title", "Unknown");
    info.uploader = j.value("uploader", "Unknown");
    info.url = j.value("webpage_url", "");
    
    if (j.contains("_type") && j["_type"] == "playlist") {
        // It's a playlist
        info.track_count = j["entries"].size();
        
        for (const auto& entry : j["entries"]) {
            TrackInfo track;
            track.title = entry.value("title", "Unknown");
            track.url = entry.value("url", "");
            track.id = entry.value("id", "");
            info.tracks.push_back(track);
        }
    } else {
        // Single track
        info.track_count = 1;
        TrackInfo track;
        track.title = info.title;
        track.url = info.url;
        track.id = j.value("id", "");
        info.tracks.push_back(track);
    }
    
    return info;
}

std::vector<std::string> Playlist::build_info_command(
    const std::string& url,
    const std::optional<std::string>& oauth_token
) {
    std::vector<std::string> args = {
        "-J",  // Output JSON
        "--flat-playlist",  // Fast: only get URLs, not full metadata
        "--no-warnings",
        url
    };
    
    if (oauth_token) {
        args.push_back("--add-header");
        args.push_back(fmt::format("Authorization: OAuth {}", *oauth_token));
    }
    
    return args;
}

} // namespace souncdown
