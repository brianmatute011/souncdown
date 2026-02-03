/**
 * @file progress_bar.cpp
 * @brief Progress bar implementation
 */

#include "souncdown/progress_bar.hpp"
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <fmt/core.h>

using namespace indicators;

namespace souncdown {

// Static member initialization
bool ProgressBar::enabled_ = false;

ProgressBar::ProgressBar(std::size_t total, const std::string& description)
    : total_(total), description_(description) {
    
    if (!enabled_) {
        return;
    }
    
    bar_ = std::make_unique<indicators::ProgressBar>(
        option::BarWidth{50},
        option::Start{"["},
        option::Fill{"█"},
        option::Lead{"█"},
        option::Remainder{"-"},
        option::End{"]"},
        option::PrefixText{description},  // Changed from PostfixText to PrefixText
        option::ForegroundColor{Color::green},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}},
        option::MaxProgress{total},
        option::ShowPercentage{true},
        option::ShowElapsedTime{false},
        option::ShowRemainingTime{false}
    );
}

ProgressBar::~ProgressBar() {
    if (bar_ && enabled_ && !completed_) {
        bar_->mark_as_completed();
    }
}

void ProgressBar::update(std::size_t current) {
    if (bar_ && enabled_) {
        bar_->set_progress(current);
    }
}

void ProgressBar::set_progress(std::size_t value) {
    if (bar_ && enabled_) {
        bar_->set_progress(value);
    }
}

void ProgressBar::mark_as_completed() {
    if (bar_ && enabled_ && !completed_) {
        bar_->mark_as_completed();
        completed_ = true;
    }
}

void ProgressBar::set_postfix_text(const std::string& text) {
    if (bar_ && enabled_) {
        bar_->set_option(option::PostfixText{text});
    }
}

void ProgressBar::hide_cursor() {
    if (enabled_) {
        indicators::show_console_cursor(false);
    }
}

void ProgressBar::show_cursor() {
    if (enabled_) {
        indicators::show_console_cursor(true);
    }
}

// MultiProgressBar implementation

MultiProgressBar::MultiProgressBar(std::size_t total_tracks, const std::string& playlist_name)
    : total_tracks_(total_tracks)
    , current_track_(0)
    , playlist_name_(playlist_name) {
    
    if (!ProgressBar::is_enabled()) {
        return;
    }
    
    // Create overall progress bar
    std::string overall_desc = playlist_name.empty() 
        ? "Overall Progress" 
        : fmt::format("Playlist: {}", playlist_name);
    
    overall_bar_ = std::make_unique<ProgressBar>(total_tracks, overall_desc);
    
    ProgressBar::hide_cursor();
}

void MultiProgressBar::start_track(std::size_t track_number, const std::string& track_name) {
    if (!ProgressBar::is_enabled()) {
        return;
    }
    
    current_track_ = track_number;
    
    // Create new track progress bar
    std::string track_desc = fmt::format("Track [{}/{}] {}", 
                                        track_number, 
                                        total_tracks_, 
                                        track_name);
    
    track_bar_ = std::make_unique<ProgressBar>(100, track_desc);
}

void MultiProgressBar::update_track_progress(float percentage, const std::string& speed, const std::string& eta) {
    if (!track_bar_ || !ProgressBar::is_enabled()) {
        return;
    }
    
    // Build postfix text with speed and ETA
    std::string postfix;
    if (!speed.empty() && !eta.empty()) {
        postfix = fmt::format("{:.1f}% | {} | ETA: {}", percentage, speed, eta);
    } else if (!speed.empty()) {
        postfix = fmt::format("{:.1f}% | {}", percentage, speed);
    } else {
        postfix = fmt::format("{:.1f}%", percentage);
    }
    
    track_bar_->set_postfix_text(postfix);
    track_bar_->set_progress(static_cast<std::size_t>(percentage));
}

void MultiProgressBar::complete_track() {
    if (track_bar_ && ProgressBar::is_enabled()) {
        track_bar_->mark_as_completed();
        track_bar_.reset();
    }
    
    update_overall_progress();
}

void MultiProgressBar::update_overall_progress() {
    if (overall_bar_ && ProgressBar::is_enabled()) {
        overall_bar_->set_progress(current_track_);
        
        std::string postfix = fmt::format("{}/{} tracks completed", 
                                         current_track_, 
                                         total_tracks_);
        overall_bar_->set_postfix_text(postfix);
        
        if (current_track_ >= total_tracks_) {
            overall_bar_->mark_as_completed();
            ProgressBar::show_cursor();
        }
    }
}

} // namespace souncdown
