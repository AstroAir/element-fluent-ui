#!/bin/bash
# FluentQt Cross-Platform Preset Validation Script
# Tests all available presets to ensure they work correctly

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
VALIDATE_ALL=false
VALIDATE_CURRENT_PLATFORM=true
CLEAN_AFTER=false
VERBOSE=false
DRY_RUN=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --all)
            VALIDATE_ALL=true
            VALIDATE_CURRENT_PLATFORM=false
            shift
            ;;
        --current-platform)
            VALIDATE_CURRENT_PLATFORM=true
            VALIDATE_ALL=false
            shift
            ;;
        --clean)
            CLEAN_AFTER=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --dry-run)
            DRY_RUN=true
            shift
            ;;
        --help|-h)
            echo -e "${CYAN}FluentQt Preset Validation Script${NC}"
            echo -e "${CYAN}==================================${NC}"
            echo ""
            echo -e "${YELLOW}Usage: ./scripts/validate-presets.sh [options]${NC}"
            echo ""
            echo -e "${GREEN}Options:${NC}"
            echo "  --all               Validate all presets (may fail on unsupported platforms)"
            echo "  --current-platform  Validate only current platform presets (default)"
            echo "  --clean             Clean build directories after validation"
            echo "  --verbose           Show detailed output"
            echo "  --dry-run           Show what would be validated without running"
            echo "  --help, -h          Show this help message"
            echo ""
            echo -e "${YELLOW}Examples:${NC}"
            echo "  ./scripts/validate-presets.sh                    # Validate current platform"
            echo "  ./scripts/validate-presets.sh --all              # Validate all presets"
            echo "  ./scripts/validate-presets.sh --verbose --clean  # Verbose with cleanup"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

# Ensure we're in the project root
if [[ ! -f "CMakePresets.json" ]]; then
    echo -e "${RED}Error: Please run this script from the project root directory${NC}"
    exit 1
fi

# Get platform information
PLATFORM=$(uname -s)
ARCH=$(uname -m)

echo -e "${CYAN}FluentQt Preset Validation${NC}"
echo -e "${CYAN}==========================${NC}"
echo -e "${GREEN}Platform: $PLATFORM${NC}"
echo -e "${GREEN}Architecture: $ARCH${NC}"
echo ""

# Define preset lists
GENERAL_PRESETS=("default" "debug" "release" "static" "minimal" "development")
WINDOWS_PRESETS=("windows-msys2" "windows-msvc")
MACOS_PRESETS=("macos-intel" "macos-apple-silicon")
LINUX_PRESETS=("linux-ubuntu" "linux-centos" "linux-arch" "linux-generic")

# Function to get platform-specific presets
get_platform_presets() {
    case $PLATFORM in
        Darwin*)
            echo "${MACOS_PRESETS[@]}"
            ;;
        Linux*)
            echo "${LINUX_PRESETS[@]}"
            ;;
        MINGW*|MSYS*|CYGWIN*)
            echo "${WINDOWS_PRESETS[@]}"
            ;;
        *)
            echo ""
            ;;
    esac
}

# Function to validate a single preset
validate_preset() {
    local preset="$1"
    local success=true
    
    echo -e "${BLUE}Validating preset: $preset${NC}"
    
    if [[ "$DRY_RUN" == true ]]; then
        echo -e "${YELLOW}  [DRY RUN] Would configure with preset: $preset${NC}"
        return 0
    fi
    
    # Configure step
    echo -e "  ${YELLOW}Configuring...${NC}"
    if [[ "$VERBOSE" == true ]]; then
        cmake --preset "$preset"
    else
        cmake --preset "$preset" > /dev/null 2>&1
    fi
    
    if [[ $? -ne 0 ]]; then
        echo -e "  ${RED}✗ Configuration failed${NC}"
        return 1
    fi
    
    # Build step (just configure, don't build everything)
    echo -e "  ${YELLOW}Testing build system...${NC}"
    local build_dir
    case $preset in
        debug) build_dir="build-debug" ;;
        release) build_dir="build-release" ;;
        static) build_dir="build-static" ;;
        minimal) build_dir="build-minimal" ;;
        development) build_dir="build-dev" ;;
        windows-msys2) build_dir="build-windows-msys2" ;;
        windows-msvc) build_dir="build-windows-msvc" ;;
        macos-intel) build_dir="build-macos-intel" ;;
        macos-apple-silicon) build_dir="build-macos-apple-silicon" ;;
        linux-ubuntu) build_dir="build-linux-ubuntu" ;;
        linux-centos) build_dir="build-linux-centos" ;;
        linux-arch) build_dir="build-linux-arch" ;;
        linux-generic) build_dir="build-linux-generic" ;;
        *) build_dir="build" ;;
    esac
    
    # Test if we can generate build files
    if [[ -d "$build_dir" ]]; then
        if [[ "$VERBOSE" == true ]]; then
            cmake --build "$build_dir" --target help > /dev/null
        else
            cmake --build "$build_dir" --target help > /dev/null 2>&1
        fi
        
        if [[ $? -ne 0 ]]; then
            echo -e "  ${RED}✗ Build system test failed${NC}"
            success=false
        fi
    else
        echo -e "  ${RED}✗ Build directory not created${NC}"
        success=false
    fi
    
    if [[ "$success" == true ]]; then
        echo -e "  ${GREEN}✓ Preset validation successful${NC}"
        
        # Clean up if requested
        if [[ "$CLEAN_AFTER" == true ]]; then
            echo -e "  ${YELLOW}Cleaning build directory...${NC}"
            rm -rf "$build_dir"
        fi
        
        return 0
    else
        return 1
    fi
}

# Main validation logic
PRESETS_TO_VALIDATE=()

if [[ "$VALIDATE_ALL" == true ]]; then
    PRESETS_TO_VALIDATE+=("${GENERAL_PRESETS[@]}")
    PRESETS_TO_VALIDATE+=("${WINDOWS_PRESETS[@]}")
    PRESETS_TO_VALIDATE+=("${MACOS_PRESETS[@]}")
    PRESETS_TO_VALIDATE+=("${LINUX_PRESETS[@]}")
    echo -e "${YELLOW}Validating ALL presets (some may fail on this platform)${NC}"
elif [[ "$VALIDATE_CURRENT_PLATFORM" == true ]]; then
    PRESETS_TO_VALIDATE+=("${GENERAL_PRESETS[@]}")
    PLATFORM_SPECIFIC=($(get_platform_presets))
    PRESETS_TO_VALIDATE+=("${PLATFORM_SPECIFIC[@]}")
    echo -e "${YELLOW}Validating current platform presets${NC}"
fi

echo ""

# Run validation
FAILED_PRESETS=()
SUCCESSFUL_PRESETS=()

for preset in "${PRESETS_TO_VALIDATE[@]}"; do
    if validate_preset "$preset"; then
        SUCCESSFUL_PRESETS+=("$preset")
    else
        FAILED_PRESETS+=("$preset")
    fi
    echo ""
done

# Summary
echo -e "${CYAN}Validation Summary${NC}"
echo -e "${CYAN}=================${NC}"
echo -e "${GREEN}Successful presets (${#SUCCESSFUL_PRESETS[@]}):${NC}"
for preset in "${SUCCESSFUL_PRESETS[@]}"; do
    echo -e "  ${GREEN}✓ $preset${NC}"
done

if [[ ${#FAILED_PRESETS[@]} -gt 0 ]]; then
    echo ""
    echo -e "${RED}Failed presets (${#FAILED_PRESETS[@]}):${NC}"
    for preset in "${FAILED_PRESETS[@]}"; do
        echo -e "  ${RED}✗ $preset${NC}"
    done
    echo ""
    echo -e "${YELLOW}Note: Some failures may be expected on this platform${NC}"
    exit 1
else
    echo ""
    echo -e "${GREEN}All validated presets passed!${NC}"
    exit 0
fi
