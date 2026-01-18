#!/bin/bash

set -e

echo "=== Building SuperPico Digital ==="

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "Error: PICO_SDK_PATH environment variable is not set"
    echo "Please set it to your Pico SDK location, e.g.:"
    echo "  export PICO_SDK_PATH=/path/to/pico-sdk"
    exit 1
fi

# Change to project root directory
cd "$(dirname "$0")/.."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
    cd build
    echo "Running CMake configuration..."
    cmake ..
    cd ..
else
    echo "Build directory exists, using existing configuration"
fi

# Build the project
echo "Building project..."
cd build
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
cd ..

echo ""
echo "=== Build Complete! ==="
echo ""

# List generated files
if [ -f "build/src/superpico-digital.uf2" ]; then
    echo "Generated files:"
    ls -lh build/src/superpico-digital.uf2
    echo ""
elif [ -f "build/superpico-digital.uf2" ]; then
    echo "Generated files:"
    ls -lh build/superpico-digital.uf2
    echo ""
else
    echo "Warning: Could not find superpico-digital.uf2"
    echo "Build may have succeeded but output location is unexpected"
fi
