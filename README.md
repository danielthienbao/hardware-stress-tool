# Hardware Stress Diagnostic Tool

This is a C++ project for stress testing and monitoring hardware on Windows and Linux. It was developed as a practical tool for learning about system resource management, multithreading, and cross-platform development. The tool can run CPU, memory, disk, and simulated GPU stress tests, monitor system metrics in real time, and inject various types of faults for robustness testing.

## Project Status

- Core features: implemented and tested
- Unit and integration tests: included
- Documentation: mostly complete
- Cross-platform: Windows 10/11 and Linux (tested on Ubuntu 20.04)

## Features

### Main Components
- **System Monitor**: Collects CPU, memory, and temperature metrics using platform-specific APIs
- **Stress Tester**: Runs different types of stress tests (CPU, memory, disk, simulated GPU)
- **Fault Injector**: Simulates faults like memory corruption, CPU overload, disk I/O errors, etc.
- **Logger**: Thread-safe logging to file and console
- **CLI**: Command-line interface for configuring and running tests

### Stress Test Types
- **CPU Stress Test**: Multi-threaded floating point calculations
- **Memory Stress Test**: Allocates and accesses large memory blocks
- **Disk I/O Stress Test**: Writes and reads temporary files
- **GPU Stress Test**: Simulates matrix operations (CPU-based, not real GPU)

### Fault Injection
- Memory corruption (simulated)
- CPU overload (extra threads)
- Disk I/O errors (file access issues)
- Network issues (simulated, not real network)
- Timing anomalies (delays)
- Process management (simulated process kill)

### Safety
- All stress tests are configurable (intensity, duration)
- Automatic cleanup of temporary files and threads
- Handles Ctrl+C and other interrupts for safe shutdown

## Requirements

- CMake 3.16+
- C++17 compiler (Visual Studio 2019+, GCC 7+, or Clang 6+)
- Python 3.6+ (for test scripts)
- Windows 10/11 or Linux (tested on Ubuntu 20.04)

## Building

**Windows:**
```
scripts\build.bat
```

**Linux:**
```
chmod +x scripts/build.sh
./scripts/build.sh
```

## Usage

**Show help:**
```
./build/hardware-stress-tool --help
```

**Monitor system (10 seconds):**
```
./build/hardware-stress-tool --monitor-only --duration 10000
```

**CPU stress test (30 seconds, intensity 4):**
```
./build/hardware-stress-tool --cpu-test --duration 30000 --intensity 4
```

**Memory stress test (1 minute, intensity 5):**
```
./build/hardware-stress-tool --memory-test --duration 60000 --intensity 5
```

**Disk I/O stress test (20 seconds, intensity 3):**
```
./build/hardware-stress-tool --disk-test --duration 20000 --intensity 3
```

**Run all tests (1 minute, intensity 5):**
```
./build/hardware-stress-tool --duration 60000 --intensity 5
```

**Enable fault injection:**
```
./build/hardware-stress-tool --cpu-test --fault-injection --duration 20000
```

**Custom log file:**
```
./build/hardware-stress-tool --memory-test --log-file my_test.log --duration 15000
```

## Command Line Options

```
--help, -h           Show help message
--cpu-test           Run CPU stress test
--memory-test        Run memory stress test
--gpu-test           Run GPU stress test (simulated)
--disk-test          Run disk I/O stress test
--duration <ms>      Test duration in milliseconds (default: 300000)
--intensity <1-10>   Test intensity level (default: 5)
--log-file <file>    Log file path (default: stress_test.log)
--monitor-only       Only monitor system, don't run stress tests
--fault-injection    Enable fault injection during tests
```

## Output and Logs

- Logs are written to `stress_test.log` by default (or the file you specify)
- Log levels: INFO (main events), DEBUG (metrics), WARNING, ERROR
- Example log line:
  ```
  [DEBUG] SYSTEM_METRICS CPU:45.2% MEM:67.8% TEMP:52.1°C
  ```

## Testing

- Run the foundation test to check structure and syntax:
  ```
  python scripts/test_foundation.py
  ```
- Run all tests (build, unit, integration):
  ```
  python scripts/run_tests.py
  ```
- Run the demo script for usage examples:
  ```
  python scripts/demo.py
  ```

## Project Structure

```
hardware-stress-tool/
├── include/                 # Header files
├── src/                    # Source files
├── scripts/                # Build and utility scripts
├── tests/                  # Unit tests
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
```

## Notes and Limitations

- The GPU stress test is simulated on CPU (no CUDA/OpenCL yet)
- Network and process faults are simulated, not real network attacks
- Use low intensity and short durations on laptops or old hardware
- Always monitor system temperature and be ready to stop tests
- This is a student project, not a commercial tool. Use at your own risk.

## Contributing

If you want to contribute, fork the repo and submit a pull request. Please make sure your code builds and passes all tests. Suggestions and bug reports are welcome.

## License

MIT License. See LICENSE file for details.