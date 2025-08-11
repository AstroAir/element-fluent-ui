# FluentQt Build Script for Windows
# Usage: .\scripts\build.ps1 [preset] [target]
# Example: .\scripts\build.ps1 release FluentQt

param(
    [string]$Preset = "default",
    [string]$Target = "",
    [switch]$Clean = $false,
    [switch]$Install = $false,
    [switch]$Test = $false,
    [switch]$Help = $false,
    [switch]$AutoDetect = $false,
    [switch]$ListPresets = $false
)

function Show-Help {
    Write-Host "FluentQt Build Script for Windows" -ForegroundColor Cyan
    Write-Host "=================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage: .\scripts\build.ps1 [options]" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Options:" -ForegroundColor Green
    Write-Host "  -Preset <name>    Build preset (see available presets below)"
    Write-Host "  -Target <name>    Specific target to build (optional)"
    Write-Host "  -Clean           Clean build directory before building"
    Write-Host "  -Install         Install after building"
    Write-Host "  -Test            Run tests after building"
    Write-Host "  -AutoDetect      Auto-detect and use platform-specific preset"
    Write-Host "  -ListPresets     List all available presets"
    Write-Host "  -Help            Show this help message"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\scripts\build.ps1                          # Default build"
    Write-Host "  .\scripts\build.ps1 -Preset release          # Release build"
    Write-Host "  .\scripts\build.ps1 -AutoDetect              # Auto-detect platform preset"
    Write-Host "  .\scripts\build.ps1 -Preset debug -Test      # Debug build with tests"
    Write-Host "  .\scripts\build.ps1 -Clean -Preset static    # Clean static build"
    Write-Host ""
    Write-Host "General Presets:" -ForegroundColor Green
    Write-Host "  default      - Default configuration with examples and tests"
    Write-Host "  debug        - Debug build with sanitizers"
    Write-Host "  release      - Optimized release build"
    Write-Host "  static       - Static library build"
    Write-Host "  minimal      - Minimal build with core components only"
    Write-Host "  development  - Development build with all features"
    Write-Host ""
    Write-Host "Platform-Specific Presets:" -ForegroundColor Green
    Write-Host "  windows-msys2    - Optimized for Windows MSYS2/MinGW environment"
    Write-Host "  windows-msvc     - Optimized for Windows with MSVC compiler"
}

function Get-PlatformPreset {
    # Detect Windows environment
    if ($env:MSYSTEM) {
        Write-Host "Detected: Windows MSYS2 ($env:MSYSTEM)" -ForegroundColor Green
        return "windows-msys2"
    }
    elseif ($env:VisualStudioVersion -or (Get-Command cl -ErrorAction SilentlyContinue)) {
        Write-Host "Detected: Windows MSVC" -ForegroundColor Green
        return "windows-msvc"
    }
    else {
        Write-Host "Detected: Windows (Generic)" -ForegroundColor Green
        return "default"
    }
}

function Show-Presets {
    Write-Host "Available CMake Presets:" -ForegroundColor Cyan
    Write-Host "========================" -ForegroundColor Cyan
    Write-Host ""

    if (Test-Path "CMakePresets.json") {
        Write-Host "General Presets:" -ForegroundColor Green
        Write-Host "  default      - Default configuration with examples and tests"
        Write-Host "  debug        - Debug build with sanitizers"
        Write-Host "  release      - Optimized release build"
        Write-Host "  static       - Static library build"
        Write-Host "  minimal      - Minimal build with core components only"
        Write-Host "  development  - Development build with all features"
        Write-Host ""
        Write-Host "Platform-Specific Presets:" -ForegroundColor Green
        Write-Host "  windows-msys2    - Optimized for Windows MSYS2/MinGW environment"
        Write-Host "  windows-msvc     - Optimized for Windows with MSVC compiler"
    }
    else {
        Write-Error "CMakePresets.json not found"
        exit 1
    }
}

function Test-PresetExists {
    param([string]$PresetName)

    if (Test-Path "CMakePresets.json") {
        $content = Get-Content "CMakePresets.json" -Raw
        if ($content -match "`"name`": `"$PresetName`"") {
            return $true
        }
        else {
            Write-Error "Preset '$PresetName' not found in CMakePresets.json"
            Write-Host "Use -ListPresets to see available presets" -ForegroundColor Yellow
            return $false
        }
    }
    else {
        Write-Error "CMakePresets.json not found"
        return $false
    }
}

if ($Help) {
    Show-Help
    exit 0
}

if ($ListPresets) {
    Show-Presets
    exit 0
}

# Auto-detect platform preset if requested
if ($AutoDetect) {
    $DetectedPreset = Get-PlatformPreset
    $Preset = $DetectedPreset
    Write-Host "Using auto-detected preset: $Preset" -ForegroundColor Yellow
}

# Validate preset exists
if (-not (Test-PresetExists $Preset)) {
    exit 1
}

# Ensure we're in the project root
if (!(Test-Path "CMakeLists.txt")) {
    Write-Error "Please run this script from the project root directory"
    exit 1
}

# Setup environment - detect MSYS2/MinGW installation
$msys2Paths = @(
    "D:\msys64\mingw64\bin",
    "C:\msys64\mingw64\bin",
    "$env:MSYS2_ROOT\mingw64\bin"
)

foreach ($path in $msys2Paths) {
    if (Test-Path $path) {
        $env:PATH = "$path;" + $env:PATH
        # Find GCC lib path
        $gccPath = Get-ChildItem -Path (Split-Path $path) -Directory -Name "lib\gcc\*" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($gccPath) {
            $fullGccPath = Join-Path (Split-Path $path) $gccPath
            $env:PATH = "$fullGccPath;" + $env:PATH
        }
        Write-Host "Using MinGW from: $path" -ForegroundColor Green
        break
    }
}

Write-Host "FluentQt Build Script" -ForegroundColor Cyan
Write-Host "===================" -ForegroundColor Cyan
Write-Host "Preset: $Preset" -ForegroundColor Green
if ($Target) {
    Write-Host "Target: $Target" -ForegroundColor Green
}
Write-Host ""

# Clean if requested
if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    $buildDir = switch ($Preset) {
        "debug" { "build-debug" }
        "release" { "build-release" }
        "static" { "build-static" }
        "minimal" { "build-minimal" }
        "development" { "build-dev" }
        "windows-msys2" { "build-windows-msys2" }
        "windows-msvc" { "build-windows-msvc" }
        default { "build" }
    }

    if (Test-Path $buildDir) {
        Remove-Item -Recurse -Force $buildDir
        Write-Host "Cleaned $buildDir" -ForegroundColor Green
    }
}

# Configure
Write-Host "Configuring with preset: $Preset" -ForegroundColor Yellow
$configureCmd = "cmake --preset $Preset"
Write-Host "Running: $configureCmd" -ForegroundColor Gray
Invoke-Expression $configureCmd

if ($LASTEXITCODE -ne 0) {
    Write-Error "Configuration failed"
    exit $LASTEXITCODE
}

# Build
Write-Host "Building..." -ForegroundColor Yellow
$buildCmd = "cmake --build --preset $Preset"
if ($Target) {
    $buildCmd += " --target $Target"
}
Write-Host "Running: $buildCmd" -ForegroundColor Gray
Invoke-Expression $buildCmd

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed"
    exit $LASTEXITCODE
}

# Install if requested
if ($Install) {
    Write-Host "Installing..." -ForegroundColor Yellow
    $installCmd = "cmake --install --preset $Preset"
    Write-Host "Running: $installCmd" -ForegroundColor Gray
    Invoke-Expression $installCmd

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Installation failed"
        exit $LASTEXITCODE
    }
}

# Test if requested
if ($Test) {
    Write-Host "Running tests..." -ForegroundColor Yellow
    $testCmd = "ctest --preset $Preset"
    Write-Host "Running: $testCmd" -ForegroundColor Gray
    Invoke-Expression $testCmd

    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Some tests failed"
    }
}

Write-Host ""
Write-Host "Build completed successfully!" -ForegroundColor Green
