#!/bin/bash
# build_unified_example.sh
# Build script for FluentQt Unified Component Showcase

set -e  # Exit on any error

echo "Building FluentQt Unified Component Showcase..."
echo

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found. Please install CMake."
    exit 1
fi

# Check if Qt is available
if ! command -v qmake &> /dev/null; then
    echo "ERROR: Qt not found. Please install Qt and add it to PATH."
    echo "Make sure qmake is available in your PATH."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "Building the project..."
cmake --build . --config Release

echo
echo "Build completed successfully!"
echo

echo "You can now run the unified showcase:"
echo "  ./build/UnifiedComponentShowcase"
echo
echo "Or run other examples:"
echo "  ./build/BasicExample"
echo "  ./build/AdvancedDemo"
echo "  ./build/ComprehensiveDemo"
echo

# Ask if user wants to run the showcase
read -p "Do you want to run the Unified Component Showcase now? (y/n): " run_showcase
if [[ $run_showcase =~ ^[Yy]$ ]]; then
    echo
    echo "Starting Unified Component Showcase..."
    ./UnifiedComponentShowcase &
fi

echo "Build script completed."
