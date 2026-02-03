/**
 * @file types.cpp
 * @brief Implementation of common types
 */

#include "souncdown/types.hpp"
#include <algorithm>
#include <cctype>

namespace souncdown {

std::optional<AudioFormat> audio_format_from_string(std::string_view str) noexcept {
    // Convert to lowercase for case-insensitive comparison
    std::string lower_str;
    lower_str.resize(str.size());
    std::transform(str.begin(), str.end(), lower_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower_str == "mp3")  return AudioFormat::MP3;
    if (lower_str == "wav")  return AudioFormat::WAV;
    if (lower_str == "flac") return AudioFormat::FLAC;
    if (lower_str == "aiff") return AudioFormat::AIFF;
    if (lower_str == "aac")  return AudioFormat::AAC;
    if (lower_str == "opus") return AudioFormat::OPUS;
    if (lower_str == "m4a")  return AudioFormat::M4A;
    
    return std::nullopt;
}

} // namespace souncdown
