# Hardware Stress Diagnostic Tool

A cross-platform C++ tool for stress testing your computer's hardware and monitoring system performance. Perfect for learning about system programming, multithreading, and hardware interaction.

## What does this do?

This tool helps you:
- **Stress test your CPU** with math-heavy calculations
- **Test your RAM** by allocating and checking memory
- **Stress your hard drive** with read/write operations  
- **Monitor system performance** in real-time
- **Simulate hardware faults** to test system robustness

## Quick Demo (No Building Required!)

Want to see what it does? Run the Python demo:
```bash
pip install psutil
python scripts/demo.py
```

## Building the Tool

### Windows
1. Install Visual Studio 2019 or 2022 with C++ tools
2. Open "Developer Command Prompt"
3. Run: `scripts\build.bat`

### Linux/Mac
1. Install build tools: `sudo apt install build-essential` (Ubuntu/Debian)
2. Run: `scripts/build.sh` or just `make`

## How to Use

Once built, run the executable:
```bash
# Linux/Mac
./build/stress_tool

# Windows
build\stress_tool.exe
```

You'll see a menu like this:
```
Select test type:
1. CPU Stress Test
2. Memory Stress Test
3. Disk Stress Test
4. System Monitor Demo
5. Fault Injection Demo
0. Exit
```

Just pick a number and hit Enter!

## What Each Test Does

### 1. CPU Stress Test
- Runs math calculations on all your CPU cores
- Shows a progress bar for 10 seconds
- Reports how many operations completed

### 2. Memory Stress Test  
- Allocates chunks of RAM (about 100MB)
- Fills them with random data
- Checks if the data is still correct

### 3. Disk Stress Test
- Creates temporary files on your hard drive
- Writes data and reads it back
- Verifies data integrity

### 4. System Monitor
- Shows real-time CPU, memory, and disk usage
- Updates every second for 15 seconds
- Like a simple version of Task Manager

### 5. Fault Injection Demo
- Simulates different types of system failures
- Shows how the system recovers from problems
- Good for testing software robustness

## Project Structure

```
hardware-stress-tool/
├── include/           # Header files (.h)
├── src/              # Source code (.cpp)  
├── scripts/          # Build scripts and demos
├── CMakeLists.txt    # CMake build file
├── Makefile          # Linux/Mac build file
└── README.md         # This file!
```

## Key Files Explained

- **`src/main.cpp`** - The main program with the menu
- **`src/stress_tester.cpp`** - Does the actual stress testing
- **`src/system_monitor.cpp`** - Monitors system performance
- **`src/logger.cpp`** - Handles logging to files
- **`include/*.h`** - Header files (function declarations)

## Learning Opportunities

This project demonstrates:
- **Multithreading** - Multiple CPU cores working at once
- **Cross-platform code** - Works on Windows and Linux
- **System monitoring** - Reading CPU, memory, disk stats
- **File I/O** - Reading and writing files efficiently
- **Error handling** - Dealing with system failures gracefully

## Troubleshooting

**Build fails?**
- Make sure you have a C++ compiler installed
- Try running the validation script: `python scripts/validate_structure.py`

**Program crashes?**
- Check the log file: `stress_tool.log`
- Try the Python demo first to see if it's a system issue

**High CPU usage during tests?**
- That's normal! The CPU test is supposed to max out your processor
- It only runs for 10 seconds by default

## Safety Notes

⚠️ **Important**: These tests will push your hardware hard!
- Monitor your system temperature during long tests
- Don't run on laptops without good cooling
- Stop tests if your computer gets too hot or slow
