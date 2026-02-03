# Packaging Guide for souncdown

This document provides step-by-step instructions for building and packaging souncdown as a Debian `.deb` package.

## Prerequisites

Make sure you have the following tools installed:

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    dpkg-dev \
    debhelper \
    yt-dlp \
    ffmpeg
```

## Step 1: Update Version Numbers

Before building a new package, update the version in these files:

### 1.1 Update CMakeLists.txt

Edit `CMakeLists.txt` and update the VERSION:

```cmake
project(souncdown
    VERSION 1.1.0  # <-- Change this
    DESCRIPTION "High-performance SoundCloud downloader CLI"
    LANGUAGES CXX
)
```

### 1.2 Update debian/changelog

Add a new entry at the TOP of `debian/changelog`:

```
souncdown (1.1.0) stable; urgency=medium

  * Brief description of changes
  * Another change
  * Fix or feature description

 -- Your Name <your.email@example.com>  Sun, 02 Feb 2026 06:30:00 +0000

souncdown (1.0.0) stable; urgency=medium
  ... (previous entries remain below)
```

**Format Notes:**
- Version in parentheses: `(1.1.0)`
- Date format: `Day, DD Mon YYYY HH:MM:SS +0000`
- Two spaces before `--` and after email
- Bullet points must have two leading spaces

### 1.3 Commit Version Changes

```bash
git add CMakeLists.txt debian/changelog
git commit -m "Bump version to 1.1.0"
```

## Step 2: Clean Build Directory

Start with a fresh build:

```bash
cd /path/to/souncdown
rm -rf build/*
```

## Step 3: Configure with CMake (Release Mode)

Configure the project for Release build:

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
```

**Expected output:**
```
-- The CXX compiler identification is GNU X.X.X
-- Configuring done
-- Generating done
-- Build files have been written to: .../build
```

**Note:** This step may take 2-5 minutes on first run as it downloads dependencies (fmt, spdlog, indicators).

## Step 4: Compile the Project

Build the release binary:

```bash
cmake --build . -j$(nproc)
```

**Expected output:**
```
[ 31%] Built target fmt
[ 50%] Built target spdlog
[ 82%] Built target indicators
[100%] Built target souncdown
```

**Time:** Usually 30-60 seconds with `-j$(nproc)`.

## Step 5: Test the Binary (Optional but Recommended)

Before packaging, verify the binary works:

```bash
# Check version
./souncdown --version

# Check dependencies
./souncdown --check-deps

# Test single track download
./souncdown --info "https://soundcloud.com/some-track-url"
```

## Step 6: Generate the .deb Package

Use CPack to create the Debian package:

```bash
cpack -G DEB
```

**Expected output:**
```
CPack: Create package using DEB
CPack: Install projects
CPack: - Run preinstall target for: souncdown
CPack: - Install project: souncdown []
CPack: Create package
CPack: - package: .../build/souncdown_1.1.0_amd64.deb generated.
```

**Output file:** `souncdown_<VERSION>_amd64.deb`

**Note:** If the file is named `souncdown-1.1.0-Linux.deb` instead, you need to add this line to `CMakeLists.txt`:
```cmake
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
```
Then reconfigure: `cmake ..` and regenerate: `cpack -G DEB`

## Step 7: Verify the Package

Check the package contents and metadata:

```bash
# List package info
dpkg-deb --info souncdown_1.1.0_amd64.deb

# List files in package
dpkg-deb --contents souncdown_1.1.0_amd64.deb

# Check package size
ls -lh souncdown_1.1.0_amd64.deb
```

## Step 8: Test Installation Locally

Test the package on your system:

```bash
# Install
sudo apt install ./souncdown_1.1.0_amd64.deb

# Verify installation
which souncdown
souncdown --version

# Test functionality
souncdown --check-deps

# Uninstall (if testing)
sudo apt remove souncdown
```

## Step 9: Move Package to Release Directory (Optional)

Move the .deb to a dedicated directory for organization:

```bash
mkdir -p ../releases
mv souncdown_*.deb ../releases/
ls -lh ../releases/
```

## Common Issues and Solutions

### Issue: CMake takes too long or hangs

**Solution:** Kill the process and try with verbose output:
```bash
pkill cmake
rm -rf build/*
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF .. --verbose
```

### Issue: "Could NOT find XXX" during CMake

**Solution:** CMake will automatically download missing C++ libraries (fmt, spdlog, indicators) via FetchContent. Make sure you have internet connection.

### Issue: Compilation errors about C++23

**Solution:** Update your compiler:
```bash
sudo apt install g++-11  # or newer
export CXX=g++-11
```

### Issue: .deb is too large

**Solution:** Strip the binary before packaging:
```bash
strip souncdown
# Then run cpack again
```

### Issue: Package complains about missing dependencies

**Solution:** Check `debian/control` file and ensure all dependencies are listed:
```
Depends: ${shlibs:Depends}, ${misc:Depends}, libcurl4, yt-dlp, ffmpeg
```

## Package Distribution

### Option 1: GitHub Release (Recommended)

See `RELEASE.md` for detailed instructions on creating a GitHub Release with the .deb file.

Quick summary:
1. Tag the commit: `git tag -a v1.1.0 -m "Release v1.1.0"`
2. Push tag: `git push origin v1.1.0`
3. Go to GitHub → Releases → Draft new release
4. Upload the `.deb` file as a binary asset
5. Publish the release

### Option 2: Host on Your Own Server

```bash
# Upload to your server
scp souncdown_1.1.0_amd64.deb user@yourserver.com:/var/www/downloads/

# Users can install with:
wget https://yourserver.com/downloads/souncdown_1.1.0_amd64.deb
sudo apt install ./souncdown_1.1.0_amd64.deb
```

### Option 3: PPA (Ubuntu Personal Package Archive)

For wider distribution, consider creating a PPA. This requires:
- Launchpad account
- GPG key setup
- Source package building with `debuild`
- Upload to PPA with `dput`

This is more complex and beyond the scope of this guide.

## Quick Reference: Full Build Process

```bash
# 1. Update version in CMakeLists.txt and debian/changelog
vim CMakeLists.txt debian/changelog
git add CMakeLists.txt debian/changelog
git commit -m "Bump version to 1.1.0"

# 2. Clean and build
rm -rf build/*
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
cmake --build . -j$(nproc)

# 3. Test binary
./souncdown --version
./souncdown --check-deps

# 4. Generate package
cpack -G DEB

# 5. Verify package
dpkg-deb --info souncdown_*.deb
ls -lh souncdown_*.deb

# 6. Test install (optional)
sudo apt install ./souncdown_*.deb
souncdown --version
sudo apt remove souncdown

# Done! Package ready for distribution.
```

## Files Involved in Packaging

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build configuration, version number |
| `debian/changelog` | Package version history |
| `debian/control` | Package metadata, dependencies |
| `debian/rules` | Build instructions for dpkg |
| `debian/copyright` | License information |
| `build/souncdown_*.deb` | Final package output |

## Additional Resources

- CMake Documentation: https://cmake.org/documentation/
- Debian Packaging Guide: https://www.debian.org/doc/manuals/maint-guide/
- CPack Documentation: https://cmake.org/cmake/help/latest/module/CPack.html
- Ubuntu Packaging Guide: https://packaging.ubuntu.com/html/
