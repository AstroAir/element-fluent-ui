# Cross-Platform Compatibility Guide

This document outlines the cross-platform compatibility features and best practices for FluentQt.

## Supported Platforms

FluentQt is designed to work seamlessly across:

- **Windows** (Windows 10/11, Windows Server 2019+)
- **macOS** (macOS 10.15+, Apple Silicon and Intel)
- **Linux** (Ubuntu 20.04+, Fedora 35+, openSUSE, Arch Linux)

## Platform-Specific Features

### Windows Integration

- **Fluent Design System**: Native Windows 11 styling
- **DWM Integration**: Window composition and effects
- **Windows Registry**: Theme detection and system settings
- **Screen Reader Support**: NVDA, JAWS, Windows Narrator
- **High DPI**: Automatic scaling and per-monitor DPI awareness

### macOS Integration

- **Native Look**: Follows macOS Human Interface Guidelines
- **Cocoa Integration**: Native window management
- **Dark Mode**: Automatic system theme detection
- **Accessibility**: VoiceOver support
- **Retina Displays**: High-resolution graphics support

### Linux Integration

- **Desktop Environment Support**: GNOME, KDE, XFCE, others
- **Theme Integration**: Follows system GTK/Qt themes
- **Accessibility**: Orca screen reader support
- **Wayland/X11**: Support for both display servers
- **Package Formats**: DEB, RPM, AppImage support

## Build System Compatibility

### Cross-Platform CMake Presets

FluentQt now includes comprehensive cross-platform presets that automatically optimize builds for specific platforms and toolchains:

#### Automatic Platform Detection

```bash
# Auto-detect platform and use optimized preset
./scripts/build.sh --auto-detect   # Linux/macOS
.\scripts\build.ps1 -AutoDetect    # Windows

# List all available presets
./scripts/build.sh --list-presets  # Linux/macOS
.\scripts\build.ps1 -ListPresets   # Windows
```

#### Platform-Specific Presets

**Windows:**
```powershell
# MSYS2/MinGW environment
.\scripts\build.ps1 -Preset windows-msys2

# MSVC environment
.\scripts\build.ps1 -Preset windows-msvc
```

**macOS:**
```bash
# Intel Macs
./scripts/build.sh --preset macos-intel

# Apple Silicon (M1/M2)
./scripts/build.sh --preset macos-apple-silicon
```

**Linux:**
```bash
# Ubuntu/Debian
./scripts/build.sh --preset linux-ubuntu

# CentOS/RHEL/Fedora
./scripts/build.sh --preset linux-centos

# Arch Linux
./scripts/build.sh --preset linux-arch

# Generic Linux
./scripts/build.sh --preset linux-generic
```

#### Traditional Presets

```bash
# Linux/macOS
./scripts/build.sh --preset release

# Windows
.\scripts\build.ps1 -Preset release
```

### Platform-Specific Build Options

```cmake
# Windows-specific options
if(WIN32)
    set(FLUENTQT_ENABLE_DWM ON)
    set(FLUENTQT_ENABLE_REGISTRY ON)
endif()

# macOS-specific options
if(APPLE)
    set(FLUENTQT_ENABLE_COCOA ON)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
endif()

# Linux-specific options
if(UNIX AND NOT APPLE)
    set(FLUENTQT_ENABLE_GTK_INTEGRATION ON)
    set(FLUENTQT_ENABLE_WAYLAND ON)
endif()
```

## Path Handling

FluentQt uses Qt's cross-platform path handling:

```cpp
// ✅ Correct - Cross-platform
QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
QString filePath = configPath + QDir::separator() + "config.json";

// ❌ Incorrect - Platform-specific
QString filePath = "C:\\Users\\User\\config.json";  // Windows only
QString filePath = "/home/user/config.json";        // Linux only
```

## Compiler Compatibility

### Supported Compilers

- **Windows**: MSVC 2019+, MinGW-w64 (GCC 11+), Clang 14+
- **macOS**: Xcode 13+ (Clang 14+), Homebrew GCC 11+
- **Linux**: GCC 11+, Clang 14+

### Compiler-Specific Settings

The build system automatically configures compiler-specific options:

```cmake
if(MSVC)
    # MSVC-specific optimizations
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
    # GCC/Clang optimizations
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
```

## Dependency Management

### Package Managers

FluentQt supports multiple package managers:

- **vcpkg** (Windows, Linux, macOS)
- **Conan** (Cross-platform)
- **System packages** (Linux distributions)
- **Homebrew** (macOS)

### Qt6 Installation

#### Windows
```powershell
# Via Qt Installer
# Download from https://www.qt.io/download

# Via vcpkg
vcpkg install qt6-base qt6-widgets qt6-charts

# Via MSYS2
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-widgets
```

#### macOS
```bash
# Via Homebrew
brew install qt6

# Via Qt Installer
# Download from https://www.qt.io/download
```

#### Linux (Ubuntu/Debian)
```bash
# System packages
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev qt6-charts-dev

# Via vcpkg
./vcpkg install qt6-base qt6-widgets qt6-charts
```

## Platform Detection and Validation

### Automatic Platform Detection

FluentQt includes sophisticated platform detection that automatically identifies:

- **Operating System**: Windows, macOS, Linux
- **Architecture**: x86_64, arm64 (Apple Silicon), x86
- **Toolchain**: GCC, Clang, MSVC, Apple Clang
- **Environment**: MSYS2, MinGW, native
- **Distribution**: Ubuntu, CentOS, Arch Linux, etc. (Linux)

The build system uses this information to:
- Select optimal compiler flags
- Configure platform-specific libraries
- Set appropriate build paths
- Enable platform features

### Preset Validation

Validate that your environment supports the selected presets:

```bash
# Validate current platform presets
./scripts/validate-presets.sh --current-platform    # Linux/macOS
.\scripts\validate-presets.ps1 -CurrentPlatform     # Windows

# Test all preset definitions
.\scripts\test-cross-platform-presets.ps1           # Windows

# Dry run to see what would be validated
./scripts/validate-presets.sh --dry-run             # Linux/macOS
.\scripts\validate-presets.ps1 -DryRun              # Windows
```

## Testing Cross-Platform Compatibility

### Automated Testing

Run the cross-platform test suite:

```bash
# All platforms
cmake --build build --target FluentCrossPlatformTest
ctest -R CrossPlatform

# Platform-specific preset testing
./scripts/validate-presets.sh --verbose             # Linux/macOS
.\scripts\validate-presets.ps1 -Verbose             # Windows
```

### Manual Testing Checklist

- [ ] Application starts without errors
- [ ] UI elements render correctly
- [ ] Themes follow system appearance
- [ ] File operations work correctly
- [ ] Keyboard shortcuts function properly
- [ ] Accessibility features work
- [ ] High DPI displays render correctly

## Cross-Platform Preset Troubleshooting

### Platform Detection Issues

**Issue**: Auto-detection selects wrong preset
```bash
# Solution: Manually specify the correct preset
./scripts/build.sh --preset linux-ubuntu        # Linux/macOS
.\scripts\build.ps1 -Preset windows-msys2       # Windows
```

**Issue**: Platform not detected correctly
```bash
# Solution: Check platform detection output
./scripts/build.sh --auto-detect --verbose      # Linux/macOS
.\scripts\validate-presets.ps1 -Verbose         # Windows
```

### Preset Validation Failures

**Issue**: Preset validation fails
```bash
# Solution: Check dependencies and toolchain
./scripts/validate-presets.sh --current-platform --verbose
# Install missing dependencies based on error messages
```

**Issue**: Toolchain not found
```bash
# Solution: Ensure compiler is in PATH
export PATH="/usr/bin:$PATH"                    # Linux
export PATH="/opt/homebrew/bin:$PATH"           # macOS Homebrew
$env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH # Windows MSYS2
```

### Platform-Specific Issues

**Windows MSYS2 Issues:**
- Ensure MSYSTEM environment variable is set
- Verify MinGW packages are installed
- Check PATH includes MSYS2 binaries

**macOS Issues:**
- Verify Xcode Command Line Tools are installed
- Check architecture detection for Apple Silicon
- Ensure deployment target compatibility

**Linux Issues:**
- Install distribution-specific development packages
- Verify Qt6 installation and paths
- Check for missing system libraries

## Common Issues and Solutions

### Windows Issues

**Issue**: DLL not found errors
```powershell
# Solution: Ensure Qt DLLs are in PATH or copy to output directory
Copy-Item "$env:QT_DIR\bin\*.dll" -Destination ".\build\examples\"
```

**Issue**: High DPI scaling problems
```cpp
// Solution: Enable high DPI support
QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
```

### macOS Issues

**Issue**: Code signing requirements
```bash
# Solution: Sign the application bundle
codesign --force --deep --sign - FluentQtDemo.app
```

**Issue**: Dark mode not detected
```cpp
// Solution: Use Qt's system theme detection
bool isDark = QApplication::palette().color(QPalette::Window).lightness() < 128;
```

### Linux Issues

**Issue**: Missing system libraries
```bash
# Solution: Install development packages
sudo apt install libgl1-mesa-dev libxkbcommon-dev
```

**Issue**: Wayland compatibility
```bash
# Solution: Set Qt platform plugin
export QT_QPA_PLATFORM=wayland
# or fallback to X11
export QT_QPA_PLATFORM=xcb
```

## Best Practices

1. **Use Qt's cross-platform APIs** instead of platform-specific code
2. **Test on all target platforms** regularly
3. **Use CMake presets** for consistent builds
4. **Follow platform conventions** for UI design
5. **Handle platform differences gracefully** with feature detection
6. **Use proper path separators** with `QDir::separator()`
7. **Respect system themes** and accessibility settings

## Continuous Integration

FluentQt includes CI configurations for all supported platforms:

- **GitHub Actions**: Windows (MSVC, MinGW), macOS (Intel, Apple Silicon), Linux (Ubuntu, Fedora)
- **Platform-specific tests**: Automated cross-platform compatibility testing
- **Package generation**: Platform-specific installers and packages
