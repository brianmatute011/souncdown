/**
 * @file downloader.cpp
 * @brief Downloader implementation
 */

#include "souncdown/downloader.hpp"
#include "souncdown/process.hpp"
#include "souncdown/logger.hpp"
#include "souncdown/progress_bar.hpp"
#include <fmt/core.h>
#include <regex>
#include <sstream>

namespace souncdown {

// Helper to parse yt-dlp progress output
struct YtDlpProgress {
    float percentage{0.0f};
    std::string speed;
    std::string eta;
    
    static std::optional<YtDlpProgress> parse(const std::string& line) {
        // yt-dlp progress formats:
        // [download]  45.2% of 30.91MiB at 1.23MiB/s ETA 00:12
        // [download]   0.6% of ~ 121.14KiB at    327.11B/s ETA Unknown (frag 0/163)
        static std::regex progress_regex(
            R"(\[download\]\s+(\d+\.?\d*)%.*?at\s+([^\s]+)\s+ETA\s+([^\s\(]+))"
        );
        
        std::smatch match;
        if (std::regex_search(line, match, progress_regex) && match.size() >= 4) {
            YtDlpProgress prog;
            prog.percentage = std::stof(match[1].str());
            prog.speed = match[2].str();
            prog.eta = match[3].str();
            return prog;
        }
        
        return std::nullopt;
    }
};

Downloader::Downloader(DownloadOptions options)
    : options_(std::move(options)) {}

std::expected<bool, std::string> Downloader::download(
    const std::vector<std::string>& urls,
    ProgressCallback progress_callback
) {
    is_downloading_ = true;
    should_cancel_ = false;
    
    try {
        for (const auto& url : urls) {
            if (should_cancel_) {
                return std::unexpected("Download cancelled");
            }
            
            // Get playlist info
            auto info_result = Playlist::get_info(url, options_.oauth_token);
            if (!info_result) {
                LOG_ERROR("Failed to get info for {}: {}", url, info_result.error());
                continue;
            }
            
            auto& info = *info_result;
            
            if (info.is_playlist()) {
                download_playlist(info, progress_callback);
            } else {
                download_track(info.tracks[0]);
            }
        }
        
        is_downloading_ = false;
        return true;
        
    } catch (const std::exception& e) {
        is_downloading_ = false;
        return std::unexpected(fmt::format("Exception: {}", e.what()));
    }
}

std::expected<fs::path, std::string> Downloader::download_track(
    const TrackInfo& track_info,
    std::optional<std::size_t> track_number
) {
    auto output_template = generate_output_template(track_number);
    auto cmd_args = build_download_command(track_info.url, output_template);
    
    // Create progress bar if enabled
    std::unique_ptr<ProgressBar> progress_bar;
    bool progress_completed = false;  // Track if we've already hit 100%
    
    if (ProgressBar::is_enabled()) {
        // Truncate title if too long to fit nicely
        std::string desc = track_info.title.length() > 40 
            ? track_info.title.substr(0, 37) + "..." 
            : track_info.title;
        progress_bar = std::make_unique<ProgressBar>(100, desc);
    } else {
        // Only show log if progress bar is not shown
        LOG_INFO("Downloading: {}", track_info.title);
    }
    
    // Execute with callback to capture progress
    auto result = Process::execute_with_callback(
        "yt-dlp", 
        cmd_args,
        [&progress_bar, &progress_completed](std::string_view line, bool is_stderr) {
            if (is_stderr || !progress_bar || progress_completed) return;
            
            // Parse yt-dlp progress
            std::string line_str(line);
            auto prog = YtDlpProgress::parse(line_str);
            if (prog) {
                // Stop updating once we hit 100% to avoid duplication
                if (prog->percentage >= 100.0f) {
                    progress_completed = true;
                    progress_bar->set_progress(100);
                    progress_bar->set_postfix_text("100.0% | Complete");
                    return;
                }
                
                std::string postfix = fmt::format("{:.1f}% | {} | ETA: {}", 
                                                prog->percentage, 
                                                prog->speed, 
                                                prog->eta);
                progress_bar->set_postfix_text(postfix);
                progress_bar->set_progress(static_cast<std::size_t>(prog->percentage));
            }
        }
    );
    
    if (progress_bar) {
        progress_bar->mark_as_completed();
    }
    
    if (!result.success()) {
        return std::unexpected(fmt::format("Download failed: {}", result.stderr_output));
    }
    
    return options_.output_directory / (track_info.title + ".m4a");
}

std::expected<std::size_t, std::string> Downloader::download_playlist(
    const PlaylistInfo& playlist_info,
    ProgressCallback progress_callback
) {
    LOG_INFO("Downloading playlist: {} ({} tracks)", 
             playlist_info.title, playlist_info.track_count);
    
    std::size_t max_tracks = options_.max_downloads.value_or(playlist_info.track_count);
    std::size_t downloaded = 0;
    
    // Create multi-progress bar if enabled
    std::unique_ptr<MultiProgressBar> multi_progress;
    if (ProgressBar::is_enabled()) {
        multi_progress = std::make_unique<MultiProgressBar>(max_tracks, playlist_info.title);
    }
    
    for (std::size_t i = 0; i < max_tracks && i < playlist_info.tracks.size(); ++i) {
        if (should_cancel_) break;
        
        const auto& track = playlist_info.tracks[i];
        
        // Use track ID as display name (flat-playlist doesn't include titles)
        std::string track_display = !track.id.empty() ? track.id : fmt::format("Track {}", i + 1);
        
        LOG_INFO("[{}/{}] Downloading track: {}", i + 1, max_tracks, track_display);
        
        // Start track in multi-progress bar
        if (multi_progress) {
            multi_progress->start_track(i + 1, track_display);
        }
        
        // Download with track number if numbering is enabled
        auto track_num = options_.number_files ? std::make_optional(i + 1) : std::nullopt;
        
        // Build command and download
        auto output_template = generate_output_template(track_num);
        auto cmd_args = build_download_command(track.url, output_template);
        
        bool progress_completed = false;
        std::string real_title;
        
        // Execute with callback that updates multi-progress bar and captures title
        auto result = Process::execute_with_callback(
            "yt-dlp", 
            cmd_args,
            [&multi_progress, &progress_completed, &real_title](std::string_view line, bool is_stderr) {
                if (is_stderr || !multi_progress || progress_completed) return;
                
                std::string line_str(line);
                
                // Try to capture the real title from yt-dlp output
                if (real_title.empty() && line_str.find("[download] Destination:") != std::string::npos) {
                    // Extract filename which contains the title
                    auto pos = line_str.find_last_of('/');
                    if (pos != std::string::npos) {
                        real_title = line_str.substr(pos + 1);
                    }
                }
                
                // Parse yt-dlp progress
                auto prog = YtDlpProgress::parse(line_str);
                if (prog) {
                    if (prog->percentage >= 100.0f) {
                        progress_completed = true;
                        multi_progress->update_track_progress(100.0f, "Complete", "Done");
                    } else {
                        multi_progress->update_track_progress(prog->percentage, prog->speed, prog->eta);
                    }
                }
            }
        );
        
        if (result.success()) {
            downloaded++;
            
            if (multi_progress) {
                multi_progress->complete_track();
            }
            
            if (progress_callback) {
                DownloadProgress progress;
                progress.current_track = i + 1;
                progress.total_tracks = max_tracks;
                progress.percentage = (static_cast<double>(i + 1) / max_tracks) * 100.0;
                progress.status = DownloadStatus::IN_PROGRESS;
                progress_callback(progress);
            }
        } else {
            LOG_ERROR("Failed to download track {}: {}", i + 1, result.stderr_output);
        }
    }
    
    return downloaded;
}

void Downloader::cancel() {
    should_cancel_ = true;
}

std::vector<std::string> Downloader::build_download_command(
    const std::string& url,
    const std::string& output_template
) const {
    std::vector<std::string> args = {
        "-o", output_template,
        "-f", options_.quality
    };
    
    // Add --newline for progress bar support (forces each update on new line)
    if (ProgressBar::is_enabled()) {
        args.push_back("--newline");
    }
    
    if (options_.embed_thumbnail) {
        args.push_back("--embed-thumbnail");
    }
    
    if (options_.embed_metadata) {
        args.push_back("--embed-metadata");
    }
    
    if (options_.oauth_token) {
        args.push_back("--add-header");
        args.push_back(fmt::format("Authorization: OAuth {}", *options_.oauth_token));
    }
    
    args.push_back(url);
    
    return args;
}

std::string Downloader::generate_output_template(
    std::optional<std::size_t> track_number
) const {
    std::string template_str = options_.output_directory.string() + "/";
    
    if (track_number && options_.number_files) {
        template_str += fmt::format("{:02d} - ", *track_number);
    }
    
    template_str += "%(title)s.%(ext)s";
    
    return template_str;
}

std::optional<DownloadProgress> Downloader::parse_progress_line(std::string_view line) {
    // TODO: Parse yt-dlp progress output
    return std::nullopt;
}

} // namespace souncdown
