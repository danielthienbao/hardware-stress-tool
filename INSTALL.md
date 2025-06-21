# Hardware Stress Tool - Installation Guide

This document explains how to set up and build the Hardware Stress Tool on Windows and Linux. The instructions are written for students or anyone new to C++ projects and cross-platform builds.

## Prerequisites

- **CMake 3.16 or higher**
- **C++17 compiler**
  - Windows: Visual Studio 2019 or newer (with C++ workload)
  - Linux: GCC 7+ or Clang 6+
- **Python 3.6+** (for running test scripts)

## Platform Notes

### Windows
- You need Visual Studio (Community Edition is fine) with the C++ development tools installed.
- Make sure CMake and Python are in your PATH.
- Use PowerShell or Command Prompt.

### Linux
- You need build-essential (GCC, make, etc.).
- Install CMake and Python using your package manager.
- Tested on Ubuntu 20.04, but should work on most modern distros.

## Step-by-Step Installation

### 1. Clone the Repository

```
git clone https://github.com/yourusername/hardware-stress-tool.git
cd hardware-stress-tool
```

### 2. Check Your Environment

Run the foundation test to make sure you have the right files and tools:

```
python scripts/test_foundation.py
```

If you see errors about missing CMake, Python, or a compiler, install them and try again.

### 3. Build the Project

#### Windows
```
scripts\build.bat
```

#### Linux
```
chmod +x scripts/build.sh
./scripts/build.sh
```

This will create a `build` directory and put the executables there.

### 4. Run Tests

To make sure everything works, run:
```
python scripts/run_tests.py
```
This will build, run unit tests, and check the main features.

### 5. Try the Tool

```
./build/hardware-stress-tool --help
./build/hardware-stress-tool --monitor-only --duration 2000
```

## Troubleshooting

- **CMake not found**: Install CMake and make sure it's in your PATH.
- **Compiler not found**: On Windows, open the "Developer Command Prompt for VS". On Linux, install build-essential.
- **Build errors**: Double-check you have the right compiler and CMake version. Try deleting the `build` directory and rebuilding.
- **Permission denied (Linux)**: Use `chmod +x scripts/build.sh` and `chmod +x build/hardware-stress-tool`.
- **Python errors**: Make sure you're using Python 3, not Python 2.

## Notes

- The GPU test is simulated (runs on CPU). No CUDA/OpenCL required.
- Fault injection is for learning/testing, not for real security testing.
- If you have issues, check the log files and try running the foundation test again.

## For Contributors

- Fork the repo and make a new branch for your changes.
- Make sure your code builds and passes all tests before making a pull request.
- If you add new features, update the README and add tests if possible.

## License

MIT License. See LICENSE for details. 