# Test script for FluentQt examples
# This script tests if the examples can start without crashing

# Setup environment - detect Qt installation
$qtPaths = @(
    "D:\msys64\mingw64\bin",
    "C:\msys64\mingw64\bin",
    "$env:MSYS2_ROOT\mingw64\bin",
    "$env:QT_DIR\bin",
    "$env:Qt6_DIR\bin"
)

foreach ($path in $qtPaths) {
    if (Test-Path $path) {
        $env:PATH = "$path;" + $env:PATH
        Write-Host "Using Qt from: $path" -ForegroundColor Green
        break
    }
}

$env:QT_QPA_PLATFORM = "offscreen"

$examples = @(
    ".\build\examples\FluentQtDemo.exe",
    ".\build\examples\CarouselShowcase.exe",
    ".\build\examples\TimelineShowcase.exe",
    ".\build\examples\SimpleTimeline.exe",
    ".\build\examples\ToastAndSelectDemo.exe"
)

Write-Host "Testing FluentQt Examples..."
Write-Host "=============================="

foreach ($example in $examples) {
    $name = Split-Path $example -Leaf
    Write-Host "Testing $name..." -NoNewline

    try {
        # Start the process and kill it after 2 seconds
        $process = Start-Process -FilePath $example -PassThru -WindowStyle Hidden
        Start-Sleep -Seconds 2

        if (!$process.HasExited) {
            Stop-Process -Id $process.Id -Force
            Write-Host " PASS (started successfully)" -ForegroundColor Green
        } else {
            $exitCode = $process.ExitCode
            if ($exitCode -eq 0) {
                Write-Host " PASS (exited cleanly)" -ForegroundColor Green
            } else {
                Write-Host " FAIL (exit code: $exitCode)" -ForegroundColor Red
            }
        }
    }
    catch {
        Write-Host " FAIL (exception: $($_.Exception.Message))" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "Example testing completed."
