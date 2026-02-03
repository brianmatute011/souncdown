/**
 * @file types.hpp
 * @brief Common types and enums used throughout the application
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <chrono>

namespace souncdown {

namespace fs = std::filesystem;
using namespace std::chrono_literals;

/**
 * @enum AudioFormat
 * @brief Supported audio output formats
 */
enum class AudioFormat {
    MP3,
    WAV,
    FLAC,
    AIFF,
    AAC,
    OPUS,
    M4A
};

/**
 * @enum LogLevel
 * @brief Logging verbosity levels
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

/**
 * @enum DownloadStatus
 * @brief Status of a download operation
 */
enum class DownloadStatus {
    PENDING,
    IN_PROGRESS,
    COMPLETED,
    FAILED,
    CANCELLED
};

/**
 * @struct TrackInfo
 * @brief Information about a single track
 */
struct TrackInfo {
    std::string id;
    std::string title;
    std::string artist;
    std::string url;
    std::size_t duration_seconds{0};
    std::optional<std::string> thumbnail_url;
    std::optional<std::size_t> filesize_bytes;
};

/**
 * @struct PlaylistInfo
 * @brief Information about a SoundCloud playlist
 */
struct PlaylistInfo {
    std::string id;
    std::string title;
    std::string uploader;
    std::string url;
    std::vector<TrackInfo> tracks;
    std::size_t track_count{0};
    
    /**
     * @brief Check if this represents a playlist (vs single track)
     */
    [[nodiscard]] bool is_playlist() const noexcept {
        return track_count > 1;
    }
};

/**
 * @struct DownloadOptions
 * @brief Configuration options for downloads
 */
struct DownloadOptions {
    fs::path output_directory{fs::current_path()};
    AudioFormat output_format{AudioFormat::MP3};
    std::optional<std::string> oauth_token;
    bool embed_thumbnail{true};
    bool embed_metadata{true};
    bool number_files{false};
    std::optional<std::size_t> max_downloads;
    std::size_t playlist_start{1};
    std::string quality{"bestaudio"};
    LogLevel log_level{LogLevel::INFO};
};

/**
 * @struct DownloadProgress
 * @brief Real-time download progress information
 */
struct DownloadProgress {
    std::size_t current_track{0};
    std::size_t total_tracks{0};
    std::size_t bytes_downloaded{0};
    std::size_t total_bytes{0};
    double percentage{0.0};
    DownloadStatus status{DownloadStatus::PENDING};
    std::optional<std::chrono::seconds> eta;
    
    /**
     * @brief Check if download is complete
     */
    [[nodiscard]] bool is_complete() const noexcept {
        return status == DownloadStatus::COMPLETED;
    }
    
    /**
     * @brief Check if download has failed
     */
    [[nodiscard]] bool has_failed() const noexcept {
        return status == DownloadStatus::FAILED;
    }
};

/**
 * @brief Convert AudioFormat to string representation
 * @param format The audio format enum
 * @return String representation (e.g., "mp3", "wav")
 */
[[nodiscard]] constexpr std::string_view to_string(AudioFormat format) noexcept {
    switch (format) {
        case AudioFormat::MP3:  return "mp3";
        case AudioFormat::WAV:  return "wav";
        case AudioFormat::FLAC: return "flac";
        case AudioFormat::AIFF: return "aiff";
        case AudioFormat::AAC:  return "aac";
        case AudioFormat::OPUS: return "opus";
        case AudioFormat::M4A:  return "m4a";
        default: return "unknown";
    }
}

/**
 * @brief Convert string to AudioFormat
 * @param str String representation of format
 * @return Optional AudioFormat if valid, std::nullopt otherwise
 */
[[nodiscard]] std::optional<AudioFormat> audio_format_from_string(std::string_view str) noexcept;

/**
 * @brief Exception thrown for download-related errors
 */
class DownloadError : public std::runtime_error {
public:
    explicit DownloadError(const std::string& message) 
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown for conversion-related errors
 */
class ConversionError : public std::runtime_error {
public:
    explicit ConversionError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown for playlist parsing errors
 */
class PlaylistError : public std::runtime_error {
public:
    explicit PlaylistError(const std::string& message)
        : std::runtime_error(message) {}
};

} // namespace souncdown
