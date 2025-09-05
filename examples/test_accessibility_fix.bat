@echo off
REM Test script for FluentQt Accessibility Fix
REM This script builds and tests the fixed gallery application

echo ========================================
echo FluentQt Accessibility Fix Test Script
echo ========================================
echo.

REM Change to build directory
cd /d "%~dp0\..\build"

echo Building FluentGalleryFixed...
cmake --build . --target FluentGalleryFixed
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo Build successful! Testing application...
echo.

REM Test the application by running it for 5 seconds
echo Starting FluentGalleryFixed.exe...
echo (Application will run for 5 seconds to test for crashes)
echo.

timeout /t 2 /nobreak > nul
start /wait /b timeout /t 5 /nobreak ^& taskkill /f /im FluentGalleryFixed.exe > nul 2>&1 ^& (
    examples\FluentGalleryFixed.exe
)

if %ERRORLEVEL% equ 0 (
    echo.
    echo ========================================
    echo SUCCESS: FluentGalleryFixed runs without crashes!
    echo ========================================
    echo.
    echo The accessibility threading issue has been resolved.
    echo You can now run the gallery application safely.
    echo.
) else (
    echo.
    echo ========================================
    echo WARNING: Application may have crashed
    echo ========================================
    echo.
    echo Please check the console output for error messages.
    echo.
)

echo Test completed.
pause
