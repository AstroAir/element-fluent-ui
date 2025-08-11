#!/bin/bash
# FluentQt Build Script for Linux/macOS
# Usage: ./scripts/build.sh [preset] [target]
# Example: ./scripts/build.sh release FluentQt

set -e

# Default values
PRESET="default"
TARGET=""
CLEAN=false
INSTALL=false
TEST=false
HELP=false
AUTO_DETECT=false
LIST_PRESETS=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --preset)
            PRESET="$2"
            shift 2
            ;;
        --target)
            TARGET="$2"
            shift 2
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --test)
            TEST=true
            shift
            ;;
        --help|-h)
            HELP=true
            shift
            ;;
        --auto-detect)
            AUTO_DETECT=true
            shift
            ;;
        --list-presets)
            LIST_PRESETS=true
            shift
            ;;
        *)
            if [[ -z "$PRESET" || "$PRESET" == "default" ]]; then
                PRESET="$1"
            elif [[ -z "$TARGET" ]]; then
                TARGET="$1"
            fi
            shift
            ;;
    esac
done

show_help() {
    echo -e "\033[36mFluentQt Build Script for Linux/macOS\033[0m"
    echo -e "\033[36m====================================\033[0m"
    echo ""
    echo -e "\033[33mUsage: ./scripts/build.sh [options]\033[0m"
    echo ""
    echo -e "\033[32mOptions:\033[0m"
    echo "  --preset <name>   Build preset (see available presets below)"
    echo "  --target <name>   Specific target to build (optional)"
    echo "  --clean          Clean build directory before building"
    echo "  --install        Install after building"
    echo "  --test           Run tests after building"
    echo "  --auto-detect    Auto-detect and use platform-specific preset"
    echo "  --list-presets   List all available presets"
    echo "  --help, -h       Show this help message"
    echo ""
    echo -e "\033[33mExamples:\033[0m"
    echo "  ./scripts/build.sh                           # Default build"
    echo "  ./scripts/build.sh --preset release          # Release build"
    echo "  ./scripts/build.sh --auto-detect             # Auto-detect platform preset"
    echo "  ./scripts/build.sh --preset debug --test     # Debug build with tests"
    echo "  ./scripts/build.sh --clean --preset static   # Clean static build"
    echo ""
    echo -e "\033[32mGeneral Presets:\033[0m"
    echo "  default      - Default configuration with examples and tests"
    echo "  debug        - Debug build with sanitizers"
    echo "  release      - Optimized release build"
    echo "  static       - Static library build"
    echo "  minimal      - Minimal build with core components only"
    echo "  development  - Development build with all features"
    echo ""
    echo -e "\033[32mPlatform-Specific Presets:\033[0m"
    echo "  macos-intel         - Optimized for macOS on Intel processors"
    echo "  macos-apple-silicon - Optimized for macOS on Apple Silicon (M1/M2)"
    echo "  linux-ubuntu        - Optimized for Ubuntu/Debian distributions"
    echo "  linux-centos        - Optimized for CentOS/RHEL/Fedora distributions"
    echo "  linux-arch          - Optimized for Arch Linux and derivatives"
    echo "  linux-generic       - Generic Linux build for any distribution"
}

# Platform detection function
detect_platform_preset() {
    local platform=$(uname -s)
    local arch=$(uname -m)
    local preset="default"

    case $platform in
        Darwin*)
            if [[ "$arch" == "arm64" ]]; then
                preset="macos-apple-silicon"
                echo -e "\033[32mDetected: macOS Apple Silicon\033[0m"
            else
                preset="macos-intel"
                echo -e "\033[32mDetected: macOS Intel\033[0m"
            fi
            ;;
        Linux*)
            # Try to detect Linux distribution
            if [[ -f "/etc/os-release" ]]; then
                source /etc/os-release
                case $ID in
                    ubuntu|debian)
                        preset="linux-ubuntu"
                        echo -e "\033[32mDetected: $PRETTY_NAME (Ubuntu/Debian-based)\033[0m"
                        ;;
                    centos|rhel|fedora)
                        preset="linux-centos"
                        echo -e "\033[32mDetected: $PRETTY_NAME (Red Hat-based)\033[0m"
                        ;;
                    arch|manjaro)
                        preset="linux-arch"
                        echo -e "\033[32mDetected: $PRETTY_NAME (Arch-based)\033[0m"
                        ;;
                    *)
                        preset="linux-generic"
                        echo -e "\033[32mDetected: $PRETTY_NAME (Generic Linux)\033[0m"
                        ;;
                esac
            else
                preset="linux-generic"
                echo -e "\033[32mDetected: Linux (Generic)\033[0m"
            fi
            ;;
        *)
            preset="default"
            echo -e "\033[33mWarning: Unknown platform $platform, using default preset\033[0m"
            ;;
    esac

    echo "$preset"
}

# List available presets function
list_presets() {
    echo -e "\033[36mAvailable CMake Presets:\033[0m"
    echo -e "\033[36m========================\033[0m"
    echo ""

    if [[ -f "CMakePresets.json" ]]; then
        # Extract preset names and descriptions from CMakePresets.json
        echo -e "\033[32mGeneral Presets:\033[0m"
        echo "  default      - Default configuration with examples and tests"
        echo "  debug        - Debug build with sanitizers"
        echo "  release      - Optimized release build"
        echo "  static       - Static library build"
        echo "  minimal      - Minimal build with core components only"
        echo "  development  - Development build with all features"
        echo ""
        echo -e "\033[32mPlatform-Specific Presets:\033[0m"
        echo "  macos-intel         - Optimized for macOS on Intel processors"
        echo "  macos-apple-silicon - Optimized for macOS on Apple Silicon (M1/M2)"
        echo "  linux-ubuntu        - Optimized for Ubuntu/Debian distributions"
        echo "  linux-centos        - Optimized for CentOS/RHEL/Fedora distributions"
        echo "  linux-arch          - Optimized for Arch Linux and derivatives"
        echo "  linux-generic       - Generic Linux build for any distribution"
    else
        echo -e "\033[31mError: CMakePresets.json not found\033[0m"
        exit 1
    fi
}

if [[ "$HELP" == true ]]; then
    show_help
    exit 0
fi

if [[ "$LIST_PRESETS" == true ]]; then
    list_presets
    exit 0
fi

# Ensure we're in the project root
if [[ ! -f "CMakeLists.txt" ]]; then
    echo -e "\033[31mError: Please run this script from the project root directory\033[0m"
    exit 1
fi

# Auto-detect platform preset if requested
if [[ "$AUTO_DETECT" == true ]]; then
    DETECTED_PRESET=$(detect_platform_preset)
    PRESET="$DETECTED_PRESET"
    echo -e "\033[33mUsing auto-detected preset: $PRESET\033[0m"
fi

# Detect platform for display
PLATFORM=$(uname -s)
case $PLATFORM in
    Linux*)
        PLATFORM_NAME="Linux"
        ;;
    Darwin*)
        PLATFORM_NAME="macOS"
        ;;
    *)
        PLATFORM_NAME="Unknown"
        echo -e "\033[33mWarning: Unknown platform $PLATFORM\033[0m"
        ;;
esac

# Validate preset exists
validate_preset() {
    local preset_name="$1"
    if [[ -f "CMakePresets.json" ]]; then
        # Simple check if preset exists in the file
        if grep -q "\"name\": \"$preset_name\"" CMakePresets.json; then
            return 0
        else
            echo -e "\033[31mError: Preset '$preset_name' not found in CMakePresets.json\033[0m"
            echo -e "\033[33mUse --list-presets to see available presets\033[0m"
            return 1
        fi
    else
        echo -e "\033[31mError: CMakePresets.json not found\033[0m"
        return 1
    fi
}

# Validate the selected preset
if ! validate_preset "$PRESET"; then
    exit 1
fi

echo -e "\033[36mFluentQt Build Script\033[0m"
echo -e "\033[36m===================\033[0m"
echo -e "\033[32mPlatform: $PLATFORM_NAME\033[0m"
echo -e "\033[32mPreset: $PRESET\033[0m"
if [[ -n "$TARGET" ]]; then
    echo -e "\033[32mTarget: $TARGET\033[0m"
fi
echo ""

# Clean if requested
if [[ "$CLEAN" == true ]]; then
    echo -e "\033[33mCleaning build directory...\033[0m"
    case $PRESET in
        debug)
            BUILD_DIR="build-debug"
            ;;
        release)
            BUILD_DIR="build-release"
            ;;
        static)
            BUILD_DIR="build-static"
            ;;
        minimal)
            BUILD_DIR="build-minimal"
            ;;
        development)
            BUILD_DIR="build-dev"
            ;;
        macos-intel)
            BUILD_DIR="build-macos-intel"
            ;;
        macos-apple-silicon)
            BUILD_DIR="build-macos-apple-silicon"
            ;;
        linux-ubuntu)
            BUILD_DIR="build-linux-ubuntu"
            ;;
        linux-centos)
            BUILD_DIR="build-linux-centos"
            ;;
        linux-arch)
            BUILD_DIR="build-linux-arch"
            ;;
        linux-generic)
            BUILD_DIR="build-linux-generic"
            ;;
        *)
            BUILD_DIR="build"
            ;;
    esac

    if [[ -d "$BUILD_DIR" ]]; then
        rm -rf "$BUILD_DIR"
        echo -e "\033[32mCleaned $BUILD_DIR\033[0m"
    fi
fi

# Configure
echo -e "\033[33mConfiguring with preset: $PRESET\033[0m"
CONFIGURE_CMD="cmake --preset $PRESET"
echo -e "\033[90mRunning: $CONFIGURE_CMD\033[0m"
eval $CONFIGURE_CMD

# Build
echo -e "\033[33mBuilding...\033[0m"
BUILD_CMD="cmake --build --preset $PRESET"
if [[ -n "$TARGET" ]]; then
    BUILD_CMD="$BUILD_CMD --target $TARGET"
fi
echo -e "\033[90mRunning: $BUILD_CMD\033[0m"
eval $BUILD_CMD

# Install if requested
if [[ "$INSTALL" == true ]]; then
    echo -e "\033[33mInstalling...\033[0m"
    INSTALL_CMD="cmake --install --preset $PRESET"
    echo -e "\033[90mRunning: $INSTALL_CMD\033[0m"
    eval $INSTALL_CMD
fi

# Test if requested
if [[ "$TEST" == true ]]; then
    echo -e "\033[33mRunning tests...\033[0m"
    TEST_CMD="ctest --preset $PRESET"
    echo -e "\033[90mRunning: $TEST_CMD\033[0m"
    eval $TEST_CMD || echo -e "\033[33mWarning: Some tests failed\033[0m"
fi

echo ""
echo -e "\033[32mBuild completed successfully!\033[0m"
