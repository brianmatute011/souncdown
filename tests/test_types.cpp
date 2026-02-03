/**
 * @file test_types.cpp
 * @brief Unit tests for types module
 */

#include "souncdown/types.hpp"
#include <gtest/gtest.h>

using namespace souncdown;

TEST(TypesTest, AudioFormatToString) {
    EXPECT_EQ(to_string(AudioFormat::MP3), "mp3");
    EXPECT_EQ(to_string(AudioFormat::WAV), "wav");
    EXPECT_EQ(to_string(AudioFormat::FLAC), "flac");
    EXPECT_EQ(to_string(AudioFormat::AIFF), "aiff");
    EXPECT_EQ(to_string(AudioFormat::AAC), "aac");
    EXPECT_EQ(to_string(AudioFormat::OPUS), "opus");
    EXPECT_EQ(to_string(AudioFormat::M4A), "m4a");
}

TEST(TypesTest, AudioFormatFromString) {
    EXPECT_EQ(audio_format_from_string("mp3"), AudioFormat::MP3);
    EXPECT_EQ(audio_format_from_string("wav"), AudioFormat::WAV);
    EXPECT_EQ(audio_format_from_string("flac"), AudioFormat::FLAC);
    
    // Test case insensitivity
    EXPECT_EQ(audio_format_from_string("MP3"), AudioFormat::MP3);
    EXPECT_EQ(audio_format_from_string("FlAc"), AudioFormat::FLAC);
    
    // Test invalid format
    EXPECT_FALSE(audio_format_from_string("invalid").has_value());
    EXPECT_FALSE(audio_format_from_string("").has_value());
}

TEST(TypesTest, PlaylistInfoIsPlaylist) {
    PlaylistInfo single_track;
    single_track.track_count = 1;
    EXPECT_FALSE(single_track.is_playlist());
    
    PlaylistInfo playlist;
    playlist.track_count = 10;
    EXPECT_TRUE(playlist.is_playlist());
}

TEST(TypesTest, DownloadProgressIsComplete) {
    DownloadProgress progress;
    progress.status = DownloadStatus::PENDING;
    EXPECT_FALSE(progress.is_complete());
    
    progress.status = DownloadStatus::IN_PROGRESS;
    EXPECT_FALSE(progress.is_complete());
    
    progress.status = DownloadStatus::COMPLETED;
    EXPECT_TRUE(progress.is_complete());
    
    progress.status = DownloadStatus::FAILED;
    EXPECT_FALSE(progress.is_complete());
}

TEST(TypesTest, DownloadProgressHasFailed) {
    DownloadProgress progress;
    progress.status = DownloadStatus::COMPLETED;
    EXPECT_FALSE(progress.has_failed());
    
    progress.status = DownloadStatus::FAILED;
    EXPECT_TRUE(progress.has_failed());
}

TEST(TypesTest, DownloadOptions) {
    DownloadOptions options;
    
    // Test defaults
    EXPECT_EQ(options.output_format, AudioFormat::MP3);
    EXPECT_TRUE(options.embed_thumbnail);
    EXPECT_TRUE(options.embed_metadata);
    EXPECT_FALSE(options.number_files);
    EXPECT_EQ(options.playlist_start, 1);
    EXPECT_EQ(options.quality, "bestaudio");
}

TEST(TypesTest, TrackInfo) {
    TrackInfo track;
    track.title = "Test Track";
    track.artist = "Test Artist";
    track.url = "https://soundcloud.com/test";
    track.duration_seconds = 180;  // 3 minutes
    
    EXPECT_EQ(track.title, "Test Track");
    EXPECT_EQ(track.duration_seconds, 180);
    EXPECT_FALSE(track.thumbnail_url.has_value());
}

// Example of exception testing
TEST(TypesTest, DownloadError) {
    try {
        throw DownloadError("Test error");
        FAIL() << "Expected DownloadError to be thrown";
    } catch (const DownloadError& e) {
        EXPECT_STREQ(e.what(), "Test error");
    } catch (...) {
        FAIL() << "Expected DownloadError, got different exception";
    }
}

TEST(TypesTest, ConversionError) {
    try {
        throw ConversionError("Conversion failed");
        FAIL() << "Expected ConversionError to be thrown";
    } catch (const ConversionError& e) {
        EXPECT_STREQ(e.what(), "Conversion failed");
    }
}

TEST(TypesTest, PlaylistError) {
    try {
        throw PlaylistError("Playlist parsing error");
        FAIL() << "Expected PlaylistError to be thrown";
    } catch (const PlaylistError& e) {
        EXPECT_STREQ(e.what(), "Playlist parsing error");
    }
}
