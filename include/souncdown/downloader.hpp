/**
 * @file downloader.hpp
 * @brief High-level download orchestration
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include "playlist.hpp"
#include <functional>
#include <expected>
#include <atomic>

namespace souncdown {

/**
 * @class Downloader
 * @brief Main download orchestration class
 * 
 * Manages the complete download workflow including:
 * - Playlist detection and parsing
 * - Track downloading via yt-dlp
 * - Progress tracking and reporting
 * - Error handling and retries
 */
class Downloader {
public:
    /**
     * @brief Callback for progress updates
     * @param progress Current download progress
     */
    using ProgressCallback = std::function<void(const DownloadProgress& progress)>;
    
    /**
     * @brief Construct a downloader with options
     * @param options Download configuration
     */
    explicit Downloader(DownloadOptions options);
    
    /**
     * @brief Download from single or multiple URLs
     * @param urls List of SoundCloud URLs
     * @param progress_callback Optional callback for progress updates
     * @return Expected with true on success, error message on failure
     */
    [[nodiscard]] std::expected<bool, std::string> download(
        const std::vector<std::string>& urls,
        ProgressCallback progress_callback = nullptr
    );
    
    /**
     * @brief Download a single track
     * @param track_info Track information
     * @param track_number Track number in playlist (for numbering)
     * @return Expected with path to downloaded file on success
     */
    [[nodiscard]] std::expected<fs::path, std::string> download_track(
        const TrackInfo& track_info,
        std::optional<std::size_t> track_number = std::nullopt
    );
    
    /**
     * @brief Download an entire playlist
     * @param playlist_info Playlist information
     * @param progress_callback Optional progress callback
     * @return Expected with number of successfully downloaded tracks
     */
    [[nodiscard]] std::expected<std::size_t, std::string> download_playlist(
        const PlaylistInfo& playlist_info,
        ProgressCallback progress_callback = nullptr
    );
    
    /**
     * @brief Cancel an ongoing download
     */
    void cancel();
    
    /**
     * @brief Check if download is currently in progress
     */
    [[nodiscard]] bool is_downloading() const noexcept {
        return is_downloading_.load();
    }
    
    /**
     * @brief Get current download options
     */
    [[nodiscard]] const DownloadOptions& options() const noexcept {
        return options_;
    }

private:
    DownloadOptions options_;
    std::atomic<bool> is_downloading_{false};
    std::atomic<bool> should_cancel_{false};
    
    /**
     * @brief Build yt-dlp download command
     * @param url Target URL
     * @param output_template Output filename template
     * @return Command arguments vector
     */
    [[nodiscard]] std::vector<std::string> build_download_command(
        const std::string& url,
        const std::string& output_template
    ) const;
    
    /**
     * @brief Generate output filename template
     * @param track_number Optional track number for playlist numbering
     * @return Template string for yt-dlp -o option
     */
    [[nodiscard]] std::string generate_output_template(
        std::optional<std::size_t> track_number = std::nullopt
    ) const;
    
    /**
     * @brief Parse progress from yt-dlp output
     * @param line Output line from yt-dlp
     * @return Optional DownloadProgress if parseable
     */
    [[nodiscard]] static std::optional<DownloadProgress> parse_progress_line(
        std::string_view line
    );
};

} // namespace souncdown
