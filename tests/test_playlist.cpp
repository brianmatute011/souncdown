/**
 * @file test_playlist.cpp
 * @brief Unit tests for playlist functionality
 */

#include "souncdown/playlist.hpp"
#include <gtest/gtest.h>

using namespace souncdown;

TEST(PlaylistTest, IsPlaylistUrl) {
    // Playlist URLs
    EXPECT_TRUE(Playlist::is_playlist_url(
        "https://soundcloud.com/artist/sets/playlist-name"
    ));
    
    // Single track URLs
    EXPECT_FALSE(Playlist::is_playlist_url(
        "https://soundcloud.com/artist/track-name"
    ));
}

// Note: The following tests require network access and actual SoundCloud URLs
// In a production environment, these would be mocked

/*
TEST(PlaylistTest, GetInfoSingleTrack) {
    auto result = Playlist::get_info("https://soundcloud.com/valid/track");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->track_count, 1);
    EXPECT_FALSE(result->is_playlist());
}

TEST(PlaylistTest, GetInfoPlaylist) {
    auto result = Playlist::get_info("https://soundcloud.com/valid/sets/playlist");
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result->track_count, 1);
    EXPECT_TRUE(result->is_playlist());
}
*/

// Add more tests as needed
