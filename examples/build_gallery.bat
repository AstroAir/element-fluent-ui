@echo off
REM Build script for FluentQt Gallery on Windows
REM This script builds the FluentGallery example application

echo Building FluentQt Gallery...
echo.

REM Check if we're in the right directory
if not exist "FluentGallery.cpp" (
    echo Error: FluentGallery.cpp not found!
    echo Please run this script from the examples directory.
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo Error: CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo Building FluentGallery...
cmake --build . --target FluentGallery --config Release
if errorlevel 1 (
    echo Error: Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo You can now run the FluentGallery executable:
if exist "Release\FluentGallery.exe" (
    echo   Release\FluentGallery.exe
) else if exist "FluentGallery.exe" (
    echo   FluentGallery.exe
) else (
    echo   Check the build output for the executable location
)

echo.
pause
