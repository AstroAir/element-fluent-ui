#!/bin/bash
# Build script for FluentQt Gallery on Linux/macOS
# This script builds the FluentGallery example application

echo "Building FluentQt Gallery..."
echo

# Check if we're in the right directory
if [ ! -f "FluentGallery.cpp" ]; then
    echo "Error: FluentGallery.cpp not found!"
    echo "Please run this script from the examples directory."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed!"
    exit 1
fi

# Build the project
echo "Building FluentGallery..."
cmake --build . --target FluentGallery --config Release
if [ $? -ne 0 ]; then
    echo "Error: Build failed!"
    exit 1
fi

echo
echo "Build completed successfully!"
echo

# Find and display the executable location
if [ -f "FluentGallery" ]; then
    echo "You can now run the FluentGallery executable:"
    echo "  ./FluentGallery"
elif [ -f "examples/FluentGallery" ]; then
    echo "You can now run the FluentGallery executable:"
    echo "  ./examples/FluentGallery"
else
    echo "Check the build output for the executable location"
fi

echo
