#!/bin/bash

echo "Hardware Stress Diagnostic Tool - Build Script"
echo "=============================================="

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "g++ compiler not found. Please install build-essential:"
    echo "sudo apt-get install build-essential"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Check for CMake
if command -v cmake &> /dev/null; then
    echo "Using CMake build system..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
else
    echo "CMake not found, using manual compilation..."
    
    # Manual compilation
    echo "Compiling source files..."
    g++ -std=c++17 -O2 -Wall -Wextra \
        -I../include \
        ../src/main.cpp \
        ../src/logger.cpp \
        ../src/stress_tester.cpp \
        ../src/system_monitor.cpp \
        ../src/fault_injector.cpp \
        -o stress_tool \
        -pthread
fi

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Executable: build/stress_tool"
    
    # Make executable
    chmod +x stress_tool
else
    echo "Build failed!"
    exit 1
fi

cd ..
echo "Build completed." 