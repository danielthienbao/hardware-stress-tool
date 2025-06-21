# Hardware Stress Tool - Quick Start

This guide explains how to get the Hardware Stress Tool running as quickly as possible. It's written for students or anyone new to C++ projects.

## Prerequisites

- CMake 3.16+
- C++17 compiler (Visual Studio 2019+, GCC 7+, or Clang 6+)
- Python 3.6+ (for test scripts)

## 1. Clone the Repository

```
git clone https://github.com/yourusername/hardware-stress-tool.git
cd hardware-stress-tool
```

## 2. Check Your Setup

Run the foundation test to make sure you have the right files and tools:
```
python scripts/test_foundation.py
```
If you see errors, install the missing tools and try again.

## 3. Build the Project

**Windows:**
```
scripts\build.bat
```

**Linux:**
```
chmod +x scripts/build.sh
./scripts/build.sh
```

## 4. Run a Basic Test

Show help:
```
./build/hardware-stress-tool --help
```

Monitor system for 5 seconds:
```
./build/hardware-stress-tool --monitor-only --duration 5000
```

Run a CPU stress test for 10 seconds:
```
./build/hardware-stress-tool --cpu-test --duration 10000 --intensity 3
```

## 5. Other Examples

Memory stress test (15 seconds):
```
./build/hardware-stress-tool --memory-test --duration 15000 --intensity 4
```

Disk I/O stress test (10 seconds):
```
./build/hardware-stress-tool --disk-test --duration 10000 --intensity 2
```

Run all tests (20 seconds):
```
./build/hardware-stress-tool --duration 20000 --intensity 5
```

## 6. Logs and Output

- By default, logs are written to `stress_test.log`.
- You can specify a custom log file with `--log-file mylog.txt`.
- Example log line:
  ```
  [DEBUG] SYSTEM_METRICS CPU:45.2% MEM:67.8% TEMP:52.1°C
  ```

## 7. Stopping Tests

- You can stop any test early by pressing Ctrl+C.
- The tool will clean up and exit safely.

## 8. Troubleshooting

- If you get build errors, check your compiler and CMake version.
- If the tool doesn't run, make sure you are in the `build` directory and the executable exists.
- For more examples, see the demo script:
  ```
  python scripts/demo.py
  ```

## Notes

- The GPU test is simulated (runs on CPU).
- Fault injection is for learning/testing, not for real security testing.
- This is a student project. Use at your own risk. 