#!/bin/bash
# SounCdown C++ Build Script

set -e  # Exit on error

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== SounCdown C++ Build Script ===${NC}\n"

# Parse arguments
BUILD_TYPE="Release"
CLEAN=false
INSTALL=false
TESTS=true
JOBS=$(nproc)

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --no-tests)
            TESTS=false
            shift
            ;;
        --jobs|-j)
            JOBS="$2"
            shift 2
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --debug       Build in Debug mode (default: Release)"
            echo "  --clean       Clean build directory before building"
            echo "  --install     Install after building (requires sudo)"
            echo "  --no-tests    Skip building tests"
            echo "  -j, --jobs N  Number of parallel jobs (default: $(nproc))"
            echo "  -h, --help    Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

# Check dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"
missing_deps=()

if ! command -v cmake &> /dev/null; then
    missing_deps+=("cmake")
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    missing_deps+=("g++ or clang++")
fi

if ! pkg-config --exists libcurl; then
    missing_deps+=("libcurl-dev")
fi

if [ ${#missing_deps[@]} -gt 0 ]; then
    echo -e "${RED}Error: Missing dependencies: ${missing_deps[*]}${NC}"
    echo ""
    echo "Install them with:"
    echo "  sudo apt install build-essential cmake libcurl4-openssl-dev"
    exit 1
fi

echo -e "${GREEN}✓ All build dependencies found${NC}\n"

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# Configure
echo -e "${YELLOW}Configuring CMake (${BUILD_TYPE})...${NC}"
cmake_args=(
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
)

if [ "$TESTS" = false ]; then
    cmake_args+=(-DBUILD_TESTS=OFF)
fi

cmake "${cmake_args[@]}" ..

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ CMake configuration failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Configuration successful${NC}\n"

# Build
echo -e "${YELLOW}Building with $JOBS parallel jobs...${NC}"
cmake --build . -j$JOBS

if [ $? -ne 0 ]; then
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Build successful${NC}\n"

# Run tests
if [ "$TESTS" = true ]; then
    echo -e "${YELLOW}Running tests...${NC}"
    ctest --output-on-failure
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Some tests failed${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ All tests passed${NC}\n"
fi

# Install
if [ "$INSTALL" = true ]; then
    echo -e "${YELLOW}Installing (requires sudo)...${NC}"
    sudo cmake --install .
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Installation failed${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Installation successful${NC}\n"
fi

echo -e "${GREEN}=== Build Complete ===${NC}"
echo ""
echo "Binary location: $(pwd)/souncdown"
echo ""
echo "To run:"
echo "  ./souncdown --help"
echo ""
if [ "$INSTALL" = false ]; then
    echo "To install:"
    echo "  sudo cmake --install ."
    echo "  # or"
    echo "  ./build.sh --install"
fi
