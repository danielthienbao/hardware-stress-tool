# Installation Guide

This guide helps you get the Hardware Stress Diagnostic Tool running on your computer.

## Quick Start

**Just want to see what it does?** Try the Python demo first:
```bash
pip install psutil
python scripts/demo.py
```

## Windows Installation

### Step 1: Install Visual Studio
1. Download **Visual Studio Community 2022** (it's free!)
2. During installation, check **"Desktop development with C++"**
3. This gives you the C++ compiler we need

### Step 2: Build the Tool
1. Open **"Developer Command Prompt for VS 2022"** (search for it in Start menu)
2. Navigate to your project folder:
   ```cmd
   cd path\to\hardware-stress-tool
   ```
3. Run the build script:
   ```cmd
   scripts\build.bat
   ```

### Step 3: Run It!
```cmd
build\stress_tool.exe
```

## Linux Installation (Ubuntu/Debian)

### Step 1: Install Build Tools
```bash
sudo apt update
sudo apt install build-essential cmake git
```

### Step 2: Build the Tool
```bash
# Using the build script
scripts/build.sh

# Or using make
make
```

### Step 3: Run It!
```bash
./build/stress_tool
```

## macOS Installation

### Step 1: Install Xcode Command Line Tools
```bash
xcode-select --install
```

### Step 2: Build and Run
```bash
# Build
make

# Run
./build/stress_tool
```

## Alternative: Using CMake

If you have CMake installed (optional but recommended):

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./stress_tool  # Linux/Mac
stress_tool.exe  # Windows
```

## Troubleshooting

### "Command not found" errors
- **Windows**: Make sure you're using the Visual Studio Developer Command Prompt
- **Linux**: Install build-essential: `sudo apt install build-essential`
- **Mac**: Install Xcode command line tools: `xcode-select --install`

### Build fails with "No such file" errors
- Make sure you're in the right directory (should contain `CMakeLists.txt`)
- Run the validation script: `python scripts/validate_structure.py`

### "Permission denied" when running
- **Linux/Mac**: Make the file executable: `chmod +x build/stress_tool`

### High CPU usage is normal!
- The CPU stress test is supposed to max out your processor
- Tests only run for 10 seconds by default
- Monitor your system temperature during longer tests

## Testing Your Installation

Run the test suite to make sure everything works:
```bash
# Quick validation
python scripts/validate_structure.py

# Full test suite (requires working build)
python scripts/run_tests.py

# Just the demo
python scripts/demo.py
```

## What You Get

After building, you'll have:
- `build/stress_tool` (or `.exe` on Windows) - Main program
- `stress_tool.log` - Log file (created when you run tests)
- Test files in `build/` directory

## Next Steps

- Read the [README.md](../README.md) for usage instructions
- Try running different stress tests
- Look at the code in `src/` to understand how it works
- Modify and experiment!

---

**Having trouble?** The most common issue is not having a C++ compiler installed. Double-check Step 1 for your platform! 