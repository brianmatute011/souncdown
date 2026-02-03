# SounCdown

Fast SoundCloud downloader with playlist support and format conversion.

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## Quick Start

### Option 1: Install from .deb Package (Recommended) ⚡

**Download and install in one command:**
```bash
wget https://github.com/brianmatute011/souncdown/releases/download/v1.0.0/souncdown_1.0.0_amd64.deb
sudo apt install ./souncdown_1.0.0_amd64.deb
```

**Or download from web:**
1. Go to [Releases](https://github.com/brianmatute011/souncdown/releases/latest)
2. Download `souncdown_1.0.0_amd64.deb`
3. Install: `sudo apt install ./souncdown_1.0.0_amd64.deb`

**Verify installation:**
```bash
souncdown --version
souncdown --check-deps
```

**Update to newer version:**
```bash
# Download new version
wget https://github.com/brianmatute011/souncdown/releases/download/v1.X.X/souncdown_1.X.X_amd64.deb

# Upgrade (keeps your config)
sudo apt install ./souncdown_1.X.X_amd64.deb
```

**Uninstall:**
```bash
sudo apt remove souncdown
```

---

### Option 2: Build from Source

**Ubuntu/Debian:**
```bash
# 1. Install dependencies
sudo apt install build-essential cmake yt-dlp ffmpeg

# 2. Clone and build
git clone https://github.com/yourusername/souncdown.git
cd souncdown
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build -j$(nproc)

# 3. Install (optional)
sudo cmake --install build
```

**macOS:**
```bash
# 1. Install dependencies
brew install cmake yt-dlp ffmpeg

# 2. Clone and build
git clone https://github.com/yourusername/souncdown.git
cd souncdown
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build -j$(sysctl -n hw.ncpu)

# 3. Install (optional)
sudo cmake --install build
```

**First use examples:**
```bash
# Check everything is working
souncdown --check-deps

# Download a single track with progress bar
souncdown --progress https://soundcloud.com/artist/track

# Download playlist with numbered files
souncdown --progress --number https://soundcloud.com/artist/sets/playlist

# Download as FLAC format
souncdown --progress --format flac https://soundcloud.com/artist/track
```

## Features

- **Fast**: Native C++ implementation
- **Playlists**: Download entire playlists automatically
- **Progress Bars**: Real-time download progress with speed and ETA
- **Format Conversion**: MP3, FLAC, WAV, AAC, OGG, OPUS
- **Metadata**: Automatic thumbnail and metadata embedding
- **Smart Numbering**: Auto-number playlist tracks (01, 02, 03...)

## Common Options

```bash
souncdown [OPTIONS] URL

Options:
  --progress, --interactive    Show progress bar during download
  --format FORMAT             Output format (mp3, flac, wav, etc.)
  --number                    Number playlist files (01 - Song.mp3)
  --max-downloads N           Limit playlist downloads to N tracks
  --output DIR                Download directory (default: current)
  --quality QUALITY           Audio quality (default: bestaudio)
  --info                      Show track/playlist info without downloading
  --help                      Show all options
```

## Examples

### Single Tracks
```bash
# Basic download
souncdown https://soundcloud.com/artist/track

# Download as FLAC with progress
souncdown --progress --format flac URL

# Custom output directory
souncdown --output ~/Music/SoundCloud URL
```

### Playlists
```bash
# Download entire playlist
souncdown --progress --number https://soundcloud.com/artist/sets/playlist

# Download first 10 tracks only
souncdown --max-downloads 10 --number URL

# Show playlist info without downloading
souncdown --info URL
```

### Advanced
```bash
# Check if dependencies are installed
souncdown --check-deps

# Download multiple URLs from file
souncdown --from-file urls.txt

# Private tracks (requires OAuth token)
souncdown --oauth YOUR_TOKEN URL
```

## Build Options

**Debug build with sanitizers:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

**Build with tests:**
```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

**Clean rebuild:**
```bash
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build -j$(nproc)
```

## Configuration

Create `~/.config/souncdown/config.json` for default settings:

```json
{
    "output_directory": "~/Music",
    "output_format": "mp3",
    "embed_thumbnail": true,
    "number_files": false
}
```

## Troubleshooting

**Check dependencies:**
```bash
souncdown --check-deps
```

**Build issues (clean rebuild):**
```bash
rm -rf build && cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF && cmake --build build -j$(nproc)
```

**Enable verbose output:**
```bash
souncdown --verbose URL
```

## License

MIT License - see [LICENSE](LICENSE) file for details.

## Acknowledgments

- [yt-dlp](https://github.com/yt-dlp/yt-dlp) - Download engine
- [ffmpeg](https://ffmpeg.org/) - Audio processing
- [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing
- [fmt](https://github.com/fmtlib/fmt) - String formatting
- [spdlog](https://github.com/gabime/spdlog) - Logging

## Contact

Issues and PRs welcome on [GitHub](https://github.com/yourusername/souncdown)
