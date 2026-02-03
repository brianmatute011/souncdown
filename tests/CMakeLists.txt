cmake_minimum_required(VERSION 3.25)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test sources
set(TEST_SOURCES
    test_types.cpp
    test_process.cpp
    test_playlist.cpp
)

# Create test executable
add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests
    PRIVATE
        GTest::gtest_main
        GTest::gmock
        CURL::libcurl
        nlohmann_json::nlohmann_json
        fmt::fmt
        spdlog::spdlog
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Compile test sources with same flags as main project
target_compile_options(unit_tests PRIVATE -Wall -Wextra -Wpedantic)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)
