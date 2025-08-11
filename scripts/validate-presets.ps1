# FluentQt Cross-Platform Preset Validation Script for Windows
# Tests all available presets to ensure they work correctly

param(
    [switch]$All = $false,
    [switch]$CurrentPlatform = $true,
    [switch]$Clean = $false,
    [switch]$Verbose = $false,
    [switch]$DryRun = $false,
    [switch]$Help = $false
)

function Show-Help {
    Write-Host "FluentQt Preset Validation Script" -ForegroundColor Cyan
    Write-Host "==================================" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage: .\scripts\validate-presets.ps1 [options]" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Options:" -ForegroundColor Green
    Write-Host "  -All               Validate all presets (may fail on unsupported platforms)"
    Write-Host "  -CurrentPlatform   Validate only current platform presets (default)"
    Write-Host "  -Clean             Clean build directories after validation"
    Write-Host "  -Verbose           Show detailed output"
    Write-Host "  -DryRun            Show what would be validated without running"
    Write-Host "  -Help              Show this help message"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\scripts\validate-presets.ps1                    # Validate current platform"
    Write-Host "  .\scripts\validate-presets.ps1 -All              # Validate all presets"
    Write-Host "  .\scripts\validate-presets.ps1 -Verbose -Clean   # Verbose with cleanup"
}

function Get-PlatformPresets {
    # Windows-specific presets
    return @("windows-msys2", "windows-msvc")
}

function Test-PresetValidation {
    param(
        [string]$Preset
    )
    
    Write-Host "Validating preset: $Preset" -ForegroundColor Blue
    
    if ($DryRun) {
        Write-Host "  [DRY RUN] Would configure with preset: $Preset" -ForegroundColor Yellow
        return $true
    }
    
    $success = $true
    
    # Configure step
    Write-Host "  Configuring..." -ForegroundColor Yellow
    try {
        if ($Verbose) {
            cmake --preset $Preset
        } else {
            cmake --preset $Preset 2>$null | Out-Null
        }
        
        if ($LASTEXITCODE -ne 0) {
            Write-Host "  ✗ Configuration failed" -ForegroundColor Red
            return $false
        }
    }
    catch {
        Write-Host "  ✗ Configuration failed with exception: $_" -ForegroundColor Red
        return $false
    }
    
    # Build step (just test build system)
    Write-Host "  Testing build system..." -ForegroundColor Yellow
    
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
    
    # Test if we can generate build files
    if (Test-Path $buildDir) {
        try {
            if ($Verbose) {
                cmake --build $buildDir --target help | Out-Null
            } else {
                cmake --build $buildDir --target help 2>$null | Out-Null
            }
            
            if ($LASTEXITCODE -ne 0) {
                Write-Host "  ✗ Build system test failed" -ForegroundColor Red
                $success = $false
            }
        }
        catch {
            Write-Host "  ✗ Build system test failed with exception: $_" -ForegroundColor Red
            $success = $false
        }
    }
    else {
        Write-Host "  ✗ Build directory not created" -ForegroundColor Red
        $success = $false
    }
    
    if ($success) {
        Write-Host "  ✓ Preset validation successful" -ForegroundColor Green
        
        # Clean up if requested
        if ($Clean -and (Test-Path $buildDir)) {
            Write-Host "  Cleaning build directory..." -ForegroundColor Yellow
            Remove-Item -Recurse -Force $buildDir
        }
        
        return $true
    }
    else {
        return $false
    }
}

# Main script logic
if ($Help) {
    Show-Help
    exit 0
}

# Ensure we're in the project root
if (!(Test-Path "CMakePresets.json")) {
    Write-Error "Please run this script from the project root directory"
    exit 1
}

Write-Host "FluentQt Preset Validation" -ForegroundColor Cyan
Write-Host "==========================" -ForegroundColor Cyan
Write-Host "Platform: Windows" -ForegroundColor Green
Write-Host ""

# Define preset lists
$GeneralPresets = @("default", "debug", "release", "static", "minimal", "development")
$WindowsPresets = @("windows-msys2", "windows-msvc")
$MacOSPresets = @("macos-intel", "macos-apple-silicon")
$LinuxPresets = @("linux-ubuntu", "linux-centos", "linux-arch", "linux-generic")

# Determine which presets to validate
$PresetsToValidate = @()

if ($All) {
    $PresetsToValidate += $GeneralPresets
    $PresetsToValidate += $WindowsPresets
    $PresetsToValidate += $MacOSPresets
    $PresetsToValidate += $LinuxPresets
    Write-Host "Validating ALL presets (some may fail on this platform)" -ForegroundColor Yellow
}
elseif ($CurrentPlatform) {
    $PresetsToValidate += $GeneralPresets
    $PresetsToValidate += Get-PlatformPresets
    Write-Host "Validating current platform presets" -ForegroundColor Yellow
}

Write-Host ""

# Run validation
$FailedPresets = @()
$SuccessfulPresets = @()

foreach ($preset in $PresetsToValidate) {
    if (Test-PresetValidation -Preset $preset) {
        $SuccessfulPresets += $preset
    }
    else {
        $FailedPresets += $preset
    }
    Write-Host ""
}

# Summary
Write-Host "Validation Summary" -ForegroundColor Cyan
Write-Host "=================" -ForegroundColor Cyan
Write-Host "Successful presets ($($SuccessfulPresets.Count)):" -ForegroundColor Green
foreach ($preset in $SuccessfulPresets) {
    Write-Host "  ✓ $preset" -ForegroundColor Green
}

if ($FailedPresets.Count -gt 0) {
    Write-Host ""
    Write-Host "Failed presets ($($FailedPresets.Count)):" -ForegroundColor Red
    foreach ($preset in $FailedPresets) {
        Write-Host "  ✗ $preset" -ForegroundColor Red
    }
    Write-Host ""
    Write-Host "Note: Some failures may be expected on this platform" -ForegroundColor Yellow
    exit 1
}
else {
    Write-Host ""
    Write-Host "All validated presets passed!" -ForegroundColor Green
    exit 0
}
