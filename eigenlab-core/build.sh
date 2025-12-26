#!/bin/bash
# Build script for EigenLab Core WASM module

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
OUTPUT_DIR="$SCRIPT_DIR/../_wasm"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}╔═══════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║     EigenLab Core - WebAssembly Build     ║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════╝${NC}"

# Check for emscripten
if ! command -v emcc &> /dev/null; then
    echo -e "${YELLOW}Emscripten not found. Attempting to activate...${NC}"

    # Try common locations
    if [ -f "$HOME/emsdk/emsdk_env.sh" ]; then
        source "$HOME/emsdk/emsdk_env.sh"
    elif [ -f "/opt/homebrew/Cellar/emscripten/*/libexec/emsdk_env.sh" ]; then
        source /opt/homebrew/Cellar/emscripten/*/libexec/emsdk_env.sh
    else
        echo "Error: Could not find Emscripten. Please install it first."
        exit 1
    fi
fi

echo -e "${GREEN}Using Emscripten: $(emcc --version | head -1)${NC}"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake + Emscripten
echo -e "${BLUE}Configuring with CMake...${NC}"
emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_VERBOSE_MAKEFILE=OFF

# Build
echo -e "${BLUE}Building...${NC}"
emmake make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Copy artifacts
echo -e "${BLUE}Copying artifacts to $_wasm/...${NC}"
cp eigenlab-core.js "$OUTPUT_DIR/"
cp eigenlab-core.wasm "$OUTPUT_DIR/"

# Get file sizes
JS_SIZE=$(ls -lh "$OUTPUT_DIR/eigenlab-core.js" | awk '{print $5}')
WASM_SIZE=$(ls -lh "$OUTPUT_DIR/eigenlab-core.wasm" | awk '{print $5}')

echo -e "${GREEN}╔═══════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║           Build Complete!                 ║${NC}"
echo -e "${GREEN}╠═══════════════════════════════════════════╣${NC}"
echo -e "${GREEN}║  eigenlab-core.js   : ${JS_SIZE}              ${NC}"
echo -e "${GREEN}║  eigenlab-core.wasm : ${WASM_SIZE}              ${NC}"
echo -e "${GREEN}╠═══════════════════════════════════════════╣${NC}"
echo -e "${GREEN}║  Output: ${OUTPUT_DIR}${NC}"
echo -e "${GREEN}╚═══════════════════════════════════════════╝${NC}"
