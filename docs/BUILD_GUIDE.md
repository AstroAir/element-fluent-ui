# FluentQt Build Guide

This comprehensive guide explains how to build, test, and run FluentQt on all supported platforms.

## Quick Start

### Prerequisites

- **C++20 compatible compiler** (GCC 11+, Clang 14+, MSVC 2019+)
- **CMake 3.20+**
- **Qt6.5+** (Core, Widgets, Gui, PrintSupport)
- **Git** for version control

### One-Command Build

```bash
# Linux/macOS
./scripts/build.sh --preset release

# Windows
.\scripts\build.ps1 -Preset release
```

## Detailed Build Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/ElementAstro/element-fluent-ui.git
cd element-fluent-ui
```

### 2. Install Dependencies

#### Windows

**Option A: MSYS2/MinGW (Recommended)**
```powershell
# Install MSYS2 from https://www.msys2.org/
# In MSYS2 terminal:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-widgets mingw-w64-x86_64-qt6-charts
```

**Option B: vcpkg**
```powershell
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install qt6-base qt6-widgets qt6-charts --triplet x64-windows
```

**Option C: Qt Installer**
```powershell
# Download from https://www.qt.io/download
# Install Qt 6.5+ with MSVC 2019 or MinGW
```

#### macOS

**Option A: Homebrew (Recommended)**
```bash
brew install qt6 cmake ninja
```

**Option B: Qt Installer**
```bash
# Download from https://www.qt.io/download
# Install Qt 6.5+ with Xcode toolchain
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build
sudo apt install qt6-base-dev qt6-tools-dev qt6-charts-dev
sudo apt install libgl1-mesa-dev libxkbcommon-dev
```

#### Linux (Fedora)

```bash
sudo dnf install gcc-c++ cmake ninja-build
sudo dnf install qt6-qtbase-devel qt6-qttools-devel qt6-qtcharts-devel
```

### 3. Configure and Build

#### Using Build Scripts (Recommended)

```bash
# Show available options
./scripts/build.sh --help          # Linux/macOS
.\scripts\build.ps1 -Help          # Windows

# Auto-detect platform and use optimized preset
./scripts/build.sh --auto-detect   # Linux/macOS
.\scripts\build.ps1 -AutoDetect    # Windows

# List all available presets
./scripts/build.sh --list-presets  # Linux/macOS
.\scripts\build.ps1 -ListPresets   # Windows

# Platform-specific builds
./scripts/build.sh --preset macos-apple-silicon    # macOS M1/M2
./scripts/build.sh --preset linux-ubuntu           # Ubuntu/Debian
.\scripts\build.ps1 -Preset windows-msys2          # Windows MSYS2

# Development build with tests
./scripts/build.sh --preset development --test
.\scripts\build.ps1 -Preset development -Test

# Release build
./scripts/build.sh --preset release
.\scripts\build.ps1 -Preset release

# Static library build
./scripts/build.sh --preset static
.\scripts\build.ps1 -Preset static
```

#### Using CMake Directly

```bash
# Configure
cmake --preset default
# or for specific preset:
cmake --preset release

# Build
cmake --build --preset default
# or specific target:
cmake --build build --target FluentQt

# Install (optional)
cmake --install build --prefix /usr/local
```

#### Using npm Scripts

```bash
npm run build              # Default build
npm run build:release      # Release build
npm run build:debug        # Debug build
npm run test:build         # Build and run tests
```

## Build Presets

FluentQt provides several CMake presets for different use cases:

### General Presets

| Preset | Description | Use Case |
|--------|-------------|----------|
| `default` | Standard build with examples and tests | General development |
| `debug` | Debug build with sanitizers | Debugging and development |
| `release` | Optimized production build | Distribution |
| `static` | Static library build | Embedding in other projects |
| `minimal` | Core components only | Minimal footprint |
| `development` | Full-featured development build | Active development |

### Cross-Platform Presets

FluentQt now includes platform-specific presets optimized for different operating systems and toolchains:

#### Windows Presets

| Preset | Description | Toolchain | Use Case |
|--------|-------------|-----------|----------|
| `windows-msys2` | MSYS2/MinGW optimized build | GCC (MinGW) | Windows development with Unix-like tools |
| `windows-msvc` | MSVC optimized build | MSVC | Native Windows development |

#### macOS Presets

| Preset | Description | Architecture | Use Case |
|--------|-------------|--------------|----------|
| `macos-intel` | Intel Mac optimized build | x86_64 | Intel-based Macs |
| `macos-apple-silicon` | Apple Silicon optimized build | arm64 | M1/M2 Macs |

#### Linux Presets

| Preset | Description | Distribution | Use Case |
|--------|-------------|--------------|----------|
| `linux-ubuntu` | Ubuntu/Debian optimized build | Ubuntu, Debian | Debian-based distributions |
| `linux-centos` | CentOS/RHEL optimized build | CentOS, RHEL, Fedora | Red Hat-based distributions |
| `linux-arch` | Arch Linux optimized build | Arch, Manjaro | Arch-based distributions |
| `linux-generic` | Generic Linux build | Any Linux | Universal Linux compatibility |

### Preset Configuration

Each preset configures different options:

```cmake
# Example: Release preset
CMAKE_BUILD_TYPE=Release
FLUENTQT_BUILD_EXAMPLES=OFF
FLUENTQT_BUILD_TESTS=OFF
FLUENTQT_ENABLE_LTO=ON
FLUENTQT_ENABLE_CHARTS=ON
```

## Build Options

### Core Options

| Option | Default | Description |
|--------|---------|-------------|
| `FLUENTQT_BUILD_EXAMPLES` | ON | Build example applications |
| `FLUENTQT_BUILD_TESTS` | ON | Build test suite |
| `FLUENTQT_BUILD_DOCS` | OFF | Build documentation |
| `FLUENTQT_BUILD_SHARED` | ON | Build shared library |
| `FLUENTQT_INSTALL` | ON | Generate install target |

### Feature Options

| Option | Default | Description |
|--------|---------|-------------|
| `FLUENTQT_ENABLE_CHARTS` | ON | Enable chart components |
| `FLUENTQT_ENABLE_MULTIMEDIA` | OFF | Enable multimedia components |
| `FLUENTQT_ENABLE_NETWORK` | OFF | Enable network components |
| `FLUENTQT_ENABLE_LTO` | OFF | Enable Link Time Optimization |
| `FLUENTQT_ENABLE_SANITIZERS` | OFF | Enable sanitizers (Debug only) |

### Platform-Specific Configuration

Each platform preset includes optimized settings for the target environment:

#### Windows MSYS2 Preset Features
- GCC compiler with MinGW toolchain
- Native CPU optimization (`-march=native`)
- MSYS2 environment detection
- Windows-specific libraries (dwmapi, user32, gdi32)

#### macOS Presets Features
- Architecture-specific optimization (Intel vs Apple Silicon)
- macOS deployment target configuration
- Cocoa framework integration
- Code signing compatibility

#### Linux Distribution Presets Features
- Distribution-specific library paths
- Package manager compatibility
- Desktop environment integration
- Wayland/X11 support

### Preset Validation

Validate that presets work correctly on your system:

```bash
# Validate current platform presets
./scripts/validate-presets.sh --current-platform    # Linux/macOS
.\scripts\validate-presets.ps1 -CurrentPlatform     # Windows

# Validate all presets (some may fail on current platform)
./scripts/validate-presets.sh --all                 # Linux/macOS
.\scripts\validate-presets.ps1 -All                 # Windows

# Quick test of preset definitions
.\scripts\test-cross-platform-presets.ps1           # Windows
```

### Custom Configuration

```bash
# Custom build with specific options
cmake -B build-custom \
  -DFLUENTQT_BUILD_EXAMPLES=OFF \
  -DFLUENTQT_ENABLE_CHARTS=ON \
  -DFLUENTQT_ENABLE_MULTIMEDIA=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-custom
```

## Testing

### Running Tests

```bash
# All tests
ctest --preset default

# Specific test categories
ctest -R "Unit"           # Unit tests only
ctest -R "Integration"    # Integration tests only
ctest -R "Performance"    # Performance tests only

# Verbose output
ctest --preset default --verbose

# Parallel execution
ctest --preset default --parallel 4
```

### Test Categories

- **Unit Tests**: Individual component testing
- **Integration Tests**: Component interaction testing
- **Performance Tests**: Performance benchmarking
- **Accessibility Tests**: Accessibility compliance
- **Cross-Platform Tests**: Platform-specific functionality

## Examples

### Running Examples

```bash
# Build examples
cmake --build build --target examples

# Run specific example
./build/examples/FluentQtDemo          # Linux/macOS
.\build\examples\FluentQtDemo.exe      # Windows

# Test all examples
./test_examples.ps1                    # Windows
```

### Available Examples

- **FluentQtDemo**: Main demonstration application
- **CarouselShowcase**: Carousel component showcase
- **TimelineShowcase**: Timeline component showcase
- **SimpleTimeline**: Simple timeline example
- **ToastAndSelectDemo**: Toast and selection demo

## Troubleshooting

### Common Issues

**Qt6 not found**
```bash
# Set Qt6 path
export CMAKE_PREFIX_PATH="/path/to/qt6"
# or
export Qt6_DIR="/path/to/qt6/lib/cmake/Qt6"
```

**Compiler not found (Windows)**
```powershell
# Ensure compiler is in PATH
$env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
```

**Missing dependencies (Linux)**
```bash
# Install missing development packages
sudo apt install libgl1-mesa-dev libxkbcommon-dev
```

**Build fails with sanitizers**
```bash
# Disable sanitizers for release builds
cmake --preset release -DFLUENTQT_ENABLE_SANITIZERS=OFF
```

### Debug Build Issues

1. **Check compiler version**: Ensure C++20 support
2. **Verify Qt installation**: Check Qt6 components
3. **Clean build**: Remove build directory and rebuild
4. **Check logs**: Review CMake configuration output

### Performance Optimization

```bash
# Enable all optimizations
cmake --preset release \
  -DFLUENTQT_ENABLE_LTO=ON \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# Profile-guided optimization (advanced)
cmake --preset release \
  -DCMAKE_CXX_FLAGS="-fprofile-generate" \
  # Run application to generate profile data
  # Then rebuild with -fprofile-use
```

## Integration

### Using FluentQt in Your Project

**CMake Integration**
```cmake
find_package(FluentQt REQUIRED)
target_link_libraries(your_target FluentQt::FluentQt)
```

**vcpkg Integration**
```json
{
  "dependencies": ["fluentqt"]
}
```

**Conan Integration**
```python
def requirements(self):
    self.requires("fluentqt/1.0.0")
```

## Continuous Integration

FluentQt includes CI configurations for:

- **GitHub Actions**: Multi-platform builds and tests
- **Platform Matrix**: Windows (MSVC, MinGW), macOS (Intel, Apple Silicon), Linux (Ubuntu, Fedora)
- **Automated Testing**: Full test suite execution
- **Package Generation**: Platform-specific installers

For more information, see the [Cross-Platform Guide](CROSS_PLATFORM.md).
