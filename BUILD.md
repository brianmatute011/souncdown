# Build Instructions - SounCdown C++

## Quick Start (3 commands)

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
cmake --build . -j$(nproc)
```

That's it! The binary will be at `build/souncdown`

## Requirements

- **GCC 13+** or **Clang 16+** (for C++23)
- **CMake 3.20+**
- **yt-dlp** (runtime)
- **ffmpeg** (runtime)

Install on Ubuntu/Debian:
```bash
sudo apt install build-essential cmake yt-dlp ffmpeg
```

## Usage

```bash
# Test it works
./build/souncdown --check-deps

# Download a track
./build/souncdown "https://soundcloud.com/artist/track"

# See all options
./build/souncdown --help
```

## Compile Options

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### With Tests
```bash
cmake -DBUILD_TESTS=ON ..
cmake --build .
ctest
```

### Clean Rebuild
```bash
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
cmake --build . -j$(nproc)
```

## Install System-Wide

```bash
cd build
sudo cmake --install .
# Now you can run: souncdown
```

## Using build.sh Script

```bash
# Simple build
./build.sh

# Clean build with install
./build.sh --clean --install

# Debug build
./build.sh --debug
```

## Troubleshooting

**CMake too old?**
```bash
# Get newer CMake from snap
sudo snap install cmake --classic
```

**Compiler too old?**
```bash
# Install GCC 13
sudo apt install g++-13
export CXX=g++-13
```

**Dependencies auto-download**  
nlohmann/json, fmt, and spdlog are automatically downloaded by CMake if not found system-wide.

## Performance

- Binary size: ~1.1MB
- RAM usage: ~15MB during download
- No Python interpreter overhead
- Native performance

## Notes

- The project uses C++23 for `std::expected`
- All external downloads handled by yt-dlp (no libcurl needed)
- Format conversion handled by ffmpeg
- Tests use Google Test (auto-downloaded)
