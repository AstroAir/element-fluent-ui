@echo off
REM build_unified_example.bat
REM Build script for FluentQt Unified Component Showcase

echo Building FluentQt Unified Component Showcase...
echo.

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found. Please install CMake and add it to PATH.
    pause
    exit /b 1
)

REM Check if Qt is available
where qmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: Qt not found. Please install Qt and add it to PATH.
    echo Make sure qmake is available in your PATH.
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
cd build

echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    pause
    exit /b 1
)

echo Building the project...
cmake --build . --config Release
if errorlevel 1 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo You can now run the unified showcase:
echo   build\Release\UnifiedComponentShowcase.exe
echo.
echo Or run other examples:
echo   build\Release\BasicExample.exe
echo   build\Release\AdvancedDemo.exe
echo   build\Release\ComprehensiveDemo.exe
echo.

REM Ask if user wants to run the showcase
set /p run_showcase="Do you want to run the Unified Component Showcase now? (y/n): "
if /i "%run_showcase%"=="y" (
    echo.
    echo Starting Unified Component Showcase...
    start Release\UnifiedComponentShowcase.exe
)

pause
