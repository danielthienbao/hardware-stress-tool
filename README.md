# Hardware Stress Diagnostic Tool

A lightweight C++ and shell-based tool for simulating hardware stress conditions and validating system stability on Windows/Linux environments.

## Features

- **System Monitoring**: Real-time CPU, memory, temperature, and GPU monitoring
- **Logging System**: Thread-safe logging with multiple levels and file output
- **Stress Testing Framework**: Extensible framework for different types of stress tests
- **Fault Injection**: Simulate hardware failures and system stress conditions
- **Cross-Platform**: Windows and Linux support

## Quick Start

### Prerequisites
- **CMake** (3.16 or higher)
- **C++17** compatible compiler
  - Windows: Visual Studio 2019+ or MinGW-w64
  - Linux: GCC 7+ or Clang 6+

### Building

**Windows:**
```batch
scripts\build.bat
```

**Linux:**
```bash
chmod +x scripts/build.sh
./scripts/build.sh
```

### Usage

**System Monitoring:**
```bash
./hardware-stress-tool --monitor-only
```

**Stress Testing:**
```bash
# CPU stress test for 5 minutes
./hardware-stress-tool --cpu-test --duration 300000

# Memory stress test with high intensity
./hardware-stress-tool --memory-test --intensity 8

# Multiple tests with fault injection
./hardware-stress-tool --cpu-test --memory-test --fault-injection
```

**Command Line Options:**
```
--help, -h           Show help message
--cpu-test           Run CPU stress test
--memory-test        Run memory stress test
--gpu-test           Run GPU stress test
--disk-test          Run disk I/O stress test
--duration <ms>      Test duration in milliseconds (default: 300000)
--intensity <1-10>   Test intensity level (default: 5)
--log-file <file>    Log file path (default: stress_test.log)
--monitor-only       Only monitor system, don't run stress tests
--fault-injection    Enable fault injection during tests
```

## Project Structure

```
hardware-stress-tool/
├── include/                 # Header files
├── src/                    # Source files
├── scripts/               # Build and utility scripts
├── tests/                # Test files
├── CMakeLists.txt        # CMake configuration
└── README.md            # This file
```

## Testing

Run the foundation test to verify the project structure:
```bash
python scripts/validate_structure.py
```

Run a mock test to see the tool in action:
```bash
python scripts/test_foundation.py
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Disclaimer

⚠️ **Warning**: This tool is designed for testing and development purposes. Running stress tests can potentially cause system instability, data loss, or hardware damage. Use at your own risk and ensure you have proper backups before running any stress tests.
