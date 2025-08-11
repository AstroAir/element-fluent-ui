# Quick test script to verify cross-platform presets work
# This script tests basic functionality without full builds

Write-Host "Testing FluentQt Cross-Platform Presets" -ForegroundColor Cyan
Write-Host "=======================================" -ForegroundColor Cyan

# Check if CMakePresets.json exists
if (!(Test-Path "CMakePresets.json")) {
    Write-Error "CMakePresets.json not found. Please run from project root."
    exit 1
}

# Test preset existence
$presets = @(
    "default", "debug", "release", "static", "minimal", "development",
    "windows-msys2", "windows-msvc",
    "macos-intel", "macos-apple-silicon",
    "linux-ubuntu", "linux-centos", "linux-arch", "linux-generic"
)

Write-Host "Checking preset definitions..." -ForegroundColor Yellow

$content = Get-Content "CMakePresets.json" -Raw
$missingPresets = @()
$foundPresets = @()

foreach ($preset in $presets) {
    if ($content -match "`"name`": `"$preset`"") {
        $foundPresets += $preset
        Write-Host "  ✓ $preset" -ForegroundColor Green
    } else {
        $missingPresets += $preset
        Write-Host "  ✗ $preset" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  Found presets: $($foundPresets.Count)" -ForegroundColor Green
Write-Host "  Missing presets: $($missingPresets.Count)" -ForegroundColor Red

if ($missingPresets.Count -eq 0) {
    Write-Host ""
    Write-Host "All cross-platform presets are properly defined!" -ForegroundColor Green
    
    # Test platform detection
    Write-Host ""
    Write-Host "Testing platform detection..." -ForegroundColor Yellow
    
    if ($env:MSYSTEM) {
        Write-Host "  Detected: Windows MSYS2 ($env:MSYSTEM)" -ForegroundColor Green
        Write-Host "  Recommended preset: windows-msys2" -ForegroundColor Cyan
    }
    elseif ($env:VisualStudioVersion -or (Get-Command cl -ErrorAction SilentlyContinue)) {
        Write-Host "  Detected: Windows MSVC" -ForegroundColor Green
        Write-Host "  Recommended preset: windows-msvc" -ForegroundColor Cyan
    }
    else {
        Write-Host "  Detected: Windows (Generic)" -ForegroundColor Green
        Write-Host "  Recommended preset: default" -ForegroundColor Cyan
    }
    
    Write-Host ""
    Write-Host "Cross-platform preset implementation successful!" -ForegroundColor Green
    exit 0
} else {
    Write-Host ""
    Write-Host "Some presets are missing. Please check the implementation." -ForegroundColor Red
    exit 1
}
