@echo off
echo Hardware Stress Diagnostic Tool - Build Script
echo =============================================

REM Check if Visual Studio environment is set up
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Visual Studio compiler not found in PATH.
    echo Please run this script from a Visual Studio Developer Command Prompt.
    echo Or run: "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo Or run: "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo Using CMake build system...
    cmake .. -G "Visual Studio 16 2019" -A x64
    cmake --build . --config Release
) else (
    echo CMake not found, using manual compilation...
    
    REM Manual compilation
    echo Compiling source files...
    cl /std:c++17 /EHsc /I..\include ^
       ..\src\main.cpp ^
       ..\src\logger.cpp ^
       ..\src\stress_tester.cpp ^
       ..\src\system_monitor.cpp ^
       ..\src\fault_injector.cpp ^
       /Fe:stress_tool.exe ^
       /link ws2_32.lib psapi.lib pdh.lib
)

if %ERRORLEVEL% equ 0 (
    echo Build successful!
    echo Executable: build\stress_tool.exe
) else (
    echo Build failed!
)

cd ..
pause 