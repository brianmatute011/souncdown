/**
 * @file converter.hpp
 * @brief Audio format conversion using ffmpeg
 * @author SounCdown Team
 * @version 1.0.0
 */

#pragma once

#include "types.hpp"
#include <expected>
#include <functional>

namespace souncdown {

/**
 * @struct ConversionProgress
 * @brief Progress information for audio conversion
 */
struct ConversionProgress {
    fs::path input_file;
    fs::path output_file;
    double percentage{0.0};
    std::chrono::seconds elapsed{};
    std::optional<std::chrono::seconds> eta;
};

/**
 * @class Converter
 * @brief Handle audio format conversion with ffmpeg
 * 
 * Converts downloaded audio files to the desired format,
 * embeds thumbnails and metadata, and manages temporary files.
 */
class Converter {
public:
    /**
     * @brief Callback for conversion progress updates
     */
    using ProgressCallback = std::function<void(const ConversionProgress& progress)>;
    
    /**
     * @brief Construct converter with target format
     * @param target_format Desired output format
     */
    explicit Converter(AudioFormat target_format);
    
    /**
     * @brief Convert a single audio file
     * @param input_file Path to input file
     * @param output_file Optional output path (auto-generated if not provided)
     * @param progress_callback Optional progress callback
     * @return Expected with output file path on success
     * @throws ConversionError on ffmpeg failure
     */
    [[nodiscard]] std::expected<fs::path, std::string> convert(
        const fs::path& input_file,
        std::optional<fs::path> output_file = std::nullopt,
        ProgressCallback progress_callback = nullptr
    );
    
    /**
     * @brief Convert all compatible files in a directory
     * @param directory Target directory
     * @param progress_callback Optional progress callback
     * @return Expected with number of successfully converted files
     */
    [[nodiscard]] std::expected<std::size_t, std::string> convert_directory(
        const fs::path& directory,
        ProgressCallback progress_callback = nullptr
    );
    
    /**
     * @brief Embed thumbnail into audio file
     * @param audio_file Audio file path
     * @param thumbnail_file Thumbnail image path
     * @return Expected with true on success
     */
    [[nodiscard]] std::expected<bool, std::string> embed_thumbnail(
        const fs::path& audio_file,
        const fs::path& thumbnail_file
    );
    
    /**
     * @brief Check if file needs conversion
     * @param file_path File to check
     * @return true if conversion needed, false if already in target format
     */
    [[nodiscard]] bool needs_conversion(const fs::path& file_path) const;
    
    /**
     * @brief Get list of source formats that require conversion
     */
    [[nodiscard]] static std::vector<std::string> convertible_formats();

private:
    AudioFormat target_format_;
    
    /**
     * @brief Build ffmpeg conversion command
     * @param input Input file path
     * @param output Output file path
     * @return Command arguments vector
     */
    [[nodiscard]] std::vector<std::string> build_conversion_command(
        const fs::path& input,
        const fs::path& output
    ) const;
    
    /**
     * @brief Build ffmpeg command for thumbnail embedding
     * @param audio_file Audio file path
     * @param thumbnail_file Thumbnail file path
     * @param output Output file path (temporary)
     * @return Command arguments vector
     */
    [[nodiscard]] static std::vector<std::string> build_thumbnail_command(
        const fs::path& audio_file,
        const fs::path& thumbnail_file,
        const fs::path& output
    );
    
    /**
     * @brief Parse ffmpeg progress output
     * @param line Output line from ffmpeg
     * @return Optional ConversionProgress if parseable
     */
    [[nodiscard]] static std::optional<ConversionProgress> parse_ffmpeg_progress(
        std::string_view line
    );
    
    /**
     * @brief Find thumbnail file for audio file
     * @param audio_file Audio file path
     * @return Optional path to thumbnail if found
     */
    [[nodiscard]] static std::optional<fs::path> find_thumbnail(
        const fs::path& audio_file
    );
};

} // namespace souncdown
