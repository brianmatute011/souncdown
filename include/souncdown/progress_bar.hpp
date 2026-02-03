/**
 * @file progress_bar.hpp
 * @brief Interactive progress bar for downloads (like tqdm in Python)
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <memory>
#include <string>

namespace souncdown {

/**
 * @class ProgressBar
 * @brief Interactive progress bar wrapper around indicators library
 * 
 * Provides tqdm-like progress visualization for downloads with:
 * - Real-time percentage updates
 * - Download speed display
 * - ETA calculation
 * - Multi-track support for playlists
 */
class ProgressBar {
public:
    /**
     * @brief Create a progress bar for a single download
     * @param total Total size/items
     * @param description Description to show (e.g., "Downloading: Track Name")
     */
    explicit ProgressBar(std::size_t total = 100, const std::string& description = "");
    
    /**
     * @brief Destructor - ensures cursor is shown
     */
    ~ProgressBar();
    
    /**
     * @brief Update progress
     * @param current Current progress value
     */
    void update(std::size_t current);
    
    /**
     * @brief Set progress to specific value
     * @param value Progress value (0-100 for percentage mode)
     */
    void set_progress(std::size_t value);
    
    /**
     * @brief Mark as completed
     */
    void mark_as_completed();
    
    /**
     * @brief Update the postfix text (e.g., speed, ETA)
     * @param text Text to append after the bar
     */
    void set_postfix_text(const std::string& text);
    
    /**
     * @brief Check if progress bar is enabled
     */
    [[nodiscard]] static bool is_enabled() noexcept {
        return enabled_;
    }
    
    /**
     * @brief Enable/disable progress bars globally
     * @param enable True to enable, false to disable
     */
    static void set_enabled(bool enable) noexcept {
        enabled_ = enable;
    }
    
    /**
     * @brief Hide cursor (call before showing progress bars)
     */
    static void hide_cursor();
    
    /**
     * @brief Show cursor (call after all progress bars are done)
     */
    static void show_cursor();

private:
    static bool enabled_;
    std::unique_ptr<indicators::ProgressBar> bar_;
    std::size_t total_;
    std::string description_;
};

/**
 * @class MultiProgressBar
 * @brief Manage multiple progress bars for playlist downloads
 * 
 * Shows:
 * - Overall playlist progress
 * - Current track progress
 * - Track information
 */
class MultiProgressBar {
public:
    /**
     * @brief Create multi-progress bar for playlist
     * @param total_tracks Total number of tracks
     * @param playlist_name Name of the playlist
     */
    explicit MultiProgressBar(std::size_t total_tracks, const std::string& playlist_name = "");
    
    /**
     * @brief Start downloading a new track
     * @param track_number Track number (1-indexed)
     * @param track_name Name of the track
     */
    void start_track(std::size_t track_number, const std::string& track_name);
    
    /**
     * @brief Update current track progress
     * @param percentage Progress percentage (0-100)
     * @param speed Download speed string (e.g., "1.5 MiB/s")
     * @param eta ETA string (e.g., "02:30")
     */
    void update_track_progress(float percentage, const std::string& speed = "", const std::string& eta = "");
    
    /**
     * @brief Mark current track as complete
     */
    void complete_track();
    
    /**
     * @brief Update overall playlist progress
     */
    void update_overall_progress();

private:
    std::size_t total_tracks_;
    std::size_t current_track_;
    std::string playlist_name_;
    std::unique_ptr<ProgressBar> overall_bar_;
    std::unique_ptr<ProgressBar> track_bar_;
};

} // namespace souncdown
