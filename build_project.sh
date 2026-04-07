#!/bin/bash
set -e

# ==============================================================================
# CONFIGURATION
# ==============================================================================

# Set the path to your Vcpkg folder. 
if [ -z "$VCPKG_ROOT" ]; then
    VCPKG_PATH="./dependencies/vcpkg"
else
    VCPKG_PATH="$VCPKG_ROOT"
fi

# ==============================================================================
# VALIDATION
# ==============================================================================

echo "[INFO] Checking for Vcpkg at: $VCPKG_PATH"

if [ ! -f "$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake" ]; then
    echo "[ERROR] Could not find vcpkg.cmake!" 
    echo "[ERROR] Please set the VCPKG_ROOT environment variable to your correct vcpkg folder."
    exit 1
fi

# ==============================================================================
# BUILD GENERATION
# ==============================================================================

echo "[INFO] Creating build directory..."
mkdir -p build

cd build

echo "[INFO] Generating Unix Makefiles..."
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake"

if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed."
    exit $?
fi

echo ""
echo "[SUCCESS] Makefile generated in: build/"
echo "[INFO] To build the project, run: cd build && make"

