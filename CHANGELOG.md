# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.0.0] - 2026-02-02

### Added
- Initial release of SounCdown C++ version
- Fast SoundCloud downloader with C++23
- Real-time progress bars with speed and ETA display
- Playlist support with automatic detection
- Smart track numbering for playlists (01, 02, 03...)
- Multiple format conversion (MP3, FLAC, WAV, AAC, OGG, OPUS)
- Automatic metadata and thumbnail embedding
- OAuth token support for private tracks
- `--progress` / `--interactive` flag for visual progress
- `--info` flag to show track/playlist information
- `--max-downloads` to limit playlist downloads
- `--number` flag for automatic track numbering
- Configuration file support (~/.config/souncdown/config.json)
- Comprehensive error handling with std::expected
- Memory-safe implementation (no manual memory management)
- Security: No shell injection vulnerabilities

### Technical
- Built with modern C++23 standards
- CMake build system with CPack for .deb packaging
- Dependencies: yt-dlp, ffmpeg, indicators, nlohmann/json, fmt, spdlog
- Modular architecture (core, utils, cli)
- Real-time output parsing from yt-dlp
- Non-blocking I/O for streaming progress updates

### Documentation
- Comprehensive README with quick start guide
- PACKAGING.md for distribution instructions
- BUILD.md for compilation details
- Debian packaging support (control, changelog, copyright, rules)

[Unreleased]: https://github.com/yourusername/souncdown/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/yourusername/souncdown/releases/tag/v1.0.0
