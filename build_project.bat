@echo off
setlocal

:: ==============================================================================
:: CONFIGURATION
:: ==============================================================================

:: Set the path to your Vcpkg folder. 
if "%VCPKG_ROOT%"=="" (
    set VCPKG_PATH=".\dependencies\vcpkg"
) else (
    set VCPKG_PATH=%VCPKG_ROOT%
)

:: ==============================================================================
:: VALIDATION
:: ==============================================================================

echo [INFO] Checking for Vcpkg at: %VCPKG_PATH%

if not exist "%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake" (
    echo [ERROR] Could not find vcpkg.cmake! 
    echo [ERROR] Please edit this script and set VCPKG_PATH to your correct vcpkg folder.
    pause
    exit /b 1
)

:: ==============================================================================
:: BUILD GENERATION
:: ==============================================================================

echo [INFO] Creating build directory...
if not exist build (
    mkdir build
)

cd build

echo [INFO] Generating Visual Studio Solution for Rider...
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake"

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Solution generated at: build\PixelStream.sln
pause