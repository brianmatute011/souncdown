/**
 * @file converter.cpp
 * @brief Converter implementation
 */

#include "souncdown/converter.hpp"
#include "souncdown/process.hpp"
#include "souncdown/logger.hpp"
#include <fmt/core.h>

namespace souncdown {

Converter::Converter(AudioFormat target_format)
    : target_format_(target_format) {}

std::expected<fs::path, std::string> Converter::convert(
    const fs::path& input_file,
    std::optional<fs::path> output_file,
    ProgressCallback progress_callback
) {
    if (!fs::exists(input_file)) {
        return std::unexpected(fmt::format("Input file not found: {}", input_file.string()));
    }
    
    if (!needs_conversion(input_file)) {
        return input_file;  // Already in target format
    }
    
    fs::path output_path = output_file.value_or(
        input_file.parent_path() / 
        (input_file.stem().string() + "." + std::string(to_string(target_format_)))
    );
    
    // Temporary output file with proper extension for ffmpeg
    fs::path temp_output = input_file.parent_path() / 
        (input_file.stem().string() + ".tmp." + std::string(to_string(target_format_)));
    
    auto cmd_args = build_conversion_command(input_file, temp_output);
    
    LOG_INFO("Converting: {} -> {}", input_file.filename().string(), output_path.filename().string());
    
    auto result = Process::execute("ffmpeg", cmd_args);
    
    if (!result.success()) {
        if (fs::exists(temp_output)) {
            fs::remove(temp_output);
        }
        return std::unexpected(fmt::format("Conversion failed: {}", result.stderr_output));
    }
    
    // Replace original with converted
    fs::remove(input_file);
    fs::rename(temp_output, output_path);
    
    // Try to embed thumbnail
    auto thumbnail_path = find_thumbnail(output_path);
    if (thumbnail_path) {
        embed_thumbnail(output_path, *thumbnail_path);
    }
    
    return output_path;
}

std::expected<std::size_t, std::string> Converter::convert_directory(
    const fs::path& directory,
    ProgressCallback progress_callback
) {
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return std::unexpected("Invalid directory path");
    }
    
    std::size_t converted = 0;
    
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && needs_conversion(entry.path())) {
            auto result = convert(entry.path(), std::nullopt, progress_callback);
            if (result) {
                converted++;
                LOG_INFO("✓ Converted: {}", entry.path().filename().string());
            } else {
                LOG_WARN("Failed to convert {}: {}", 
                        entry.path().filename().string(), 
                        result.error());
            }
        }
    }
    
    return converted;
}

std::expected<bool, std::string> Converter::embed_thumbnail(
    const fs::path& audio_file,
    const fs::path& thumbnail_file
) {
    if (!fs::exists(audio_file) || !fs::exists(thumbnail_file)) {
        return std::unexpected("File not found");
    }
    
    fs::path temp_output = audio_file;
    temp_output += ".thumb_tmp";
    
    auto cmd_args = build_thumbnail_command(audio_file, thumbnail_file, temp_output);
    
    auto result = Process::execute("ffmpeg", cmd_args);
    
    if (!result.success()) {
        if (fs::exists(temp_output)) {
            fs::remove(temp_output);
        }
        return std::unexpected("Failed to embed thumbnail");
    }
    
    fs::remove(audio_file);
    fs::rename(temp_output, audio_file);
    fs::remove(thumbnail_file);
    
    LOG_DEBUG("  ✓ Embedded thumbnail");
    
    return true;
}

bool Converter::needs_conversion(const fs::path& file_path) const {
    auto ext = file_path.extension().string();
    if (ext.empty() || ext[0] != '.') return false;
    
    ext = ext.substr(1);  // Remove the dot
    auto target_ext = std::string(to_string(target_format_));
    
    if (ext == target_ext) return false;
    
    // Check if it's a convertible format
    const std::vector<std::string> convertible = {
        "m4a", "opus", "flac", "aiff", "aac", "webm"
    };
    
    return std::find(convertible.begin(), convertible.end(), ext) != convertible.end();
}

std::vector<std::string> Converter::convertible_formats() {
    return {"m4a", "opus", "flac", "aiff", "aac", "webm"};
}

std::vector<std::string> Converter::build_conversion_command(
    const fs::path& input,
    const fs::path& output
) const {
    return {
        "-i", input.string(),
        "-ar", "44100",
        "-ac", "2",
        "-q:a", "0",
        "-map_metadata", "0",
        "-y",
        output.string()
    };
}

std::vector<std::string> Converter::build_thumbnail_command(
    const fs::path& audio_file,
    const fs::path& thumbnail_file,
    const fs::path& output
) {
    return {
        "-i", audio_file.string(),
        "-i", thumbnail_file.string(),
        "-map", "0:a",
        "-map", "1:v",
        "-c:a", "copy",
        "-c:v", "mjpeg",
        "-map_metadata", "0",
        "-id3v2_version", "3",
        "-y",
        output.string()
    };
}

std::optional<ConversionProgress> Converter::parse_ffmpeg_progress(std::string_view line) {
    // TODO: Parse ffmpeg progress output
    return std::nullopt;
}

std::optional<fs::path> Converter::find_thumbnail(const fs::path& audio_file) {
    auto base = audio_file.parent_path() / audio_file.stem();
    
    for (const auto& ext : {".png", ".jpg", ".jpeg", ".webp"}) {
        fs::path thumb_path = base;
        thumb_path += ext;
        if (fs::exists(thumb_path)) {
            return thumb_path;
        }
    }
    
    return std::nullopt;
}

} // namespace souncdown
