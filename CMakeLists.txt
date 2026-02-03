cmake_minimum_required(VERSION 3.20)
project(souncdown
    VERSION 1.1.0
    DESCRIPTION "High-performance SoundCloud downloader CLI"
    LANGUAGES CXX
)

# C++23 for std::expected
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings and optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Werror=return-type
        -Wno-unused-parameter
    )
    
    # Optimizations for Release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
    
    # Sanitizers for Debug
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(USE_MODULES "Use C++20 modules (experimental)" OFF)

# Find dependencies
find_package(nlohmann_json 3.11.0 QUIET)
find_package(fmt QUIET)
find_package(spdlog QUIET)

# If not found, use FetchContent
include(FetchContent)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, fetching from GitHub...")
    FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

if(NOT fmt_FOUND)
    message(STATUS "fmt not found, fetching from GitHub...")
    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
    )
    FetchContent_MakeAvailable(fmt)
endif()

if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found, fetching from GitHub...")
    FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.13.0
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# indicators for progress bars (like tqdm in Python)
message(STATUS "Fetching indicators for progress bars...")
FetchContent_Declare(indicators
    GIT_REPOSITORY https://github.com/p-ranav/indicators.git
    GIT_TAG v2.3
)
FetchContent_MakeAvailable(indicators)

# Check for external dependencies
find_program(YTDLP_EXECUTABLE yt-dlp)
find_program(FFMPEG_EXECUTABLE ffmpeg)

if(NOT YTDLP_EXECUTABLE)
    message(WARNING "yt-dlp not found. Runtime dependency required.")
endif()

if(NOT FFMPEG_EXECUTABLE)
    message(WARNING "ffmpeg not found. Runtime dependency required.")
endif()

# Source files
set(SOUNCDOWN_SOURCES
    src/main.cpp
    src/core/types.cpp
    src/core/downloader.cpp
    src/core/playlist.cpp
    src/core/converter.cpp
    src/utils/process.cpp
    src/utils/logger.cpp
    src/utils/config.cpp
    src/utils/progress_bar.cpp
    src/cli/args_parser.cpp
)

set(SOUNCDOWN_HEADERS
    include/souncdown/downloader.hpp
    include/souncdown/playlist.hpp
    include/souncdown/converter.hpp
    include/souncdown/process.hpp
    include/souncdown/logger.hpp
    include/souncdown/config.hpp
    include/souncdown/args_parser.hpp
    include/souncdown/progress_bar.hpp
    include/souncdown/types.hpp
)

# Main executable
add_executable(souncdown ${SOUNCDOWN_SOURCES} ${SOUNCDOWN_HEADERS})

target_include_directories(souncdown
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(souncdown
    PRIVATE
        nlohmann_json::nlohmann_json
        fmt::fmt
        spdlog::spdlog
        indicators::indicators
)

# Compile definitions
target_compile_definitions(souncdown
    PRIVATE
        SOUNCDOWN_VERSION="${PROJECT_VERSION}"
        $<$<CONFIG:Debug>:SOUNCDOWN_DEBUG>
)

# Install targets
include(GNUInstallDirs)

install(TARGETS souncdown
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# CPack configuration for .deb package
set(CPACK_PACKAGE_NAME "souncdown")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_CONTACT "your.email@example.com")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)  # Use standard Debian naming: name_version_arch.deb
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libcurl4, yt-dlp, ffmpeg")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

include(CPack)
