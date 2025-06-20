#!/usr/bin/env python3
"""
Hardware Stress Tool - Demonstration Script
Shows how to use the tool and its various features.
"""

import os
import sys
import time
import subprocess
from pathlib import Path

def print_header(title):
    print(f"\n{'='*60}")
    print(f"  {title}")
    print(f"{'='*60}")

def print_section(title):
    print(f"\n{'-'*40}")
    print(f"  {title}")
    print(f"{'-'*40}")

def check_build_status():
    """Check if the tool is built and ready to use."""
    print_header("BUILD STATUS CHECK")
    
    executable_name = "hardware-stress-tool"
    if os.name == 'nt':  # Windows
        executable_name += ".exe"
    
    build_paths = [
        f"build/{executable_name}",
        f"Debug/{executable_name}",
        f"Release/{executable_name}",
        executable_name
    ]
    
    for path in build_paths:
        if os.path.exists(path):
            print(f"✓ Found executable: {path}")
            return path
    
    print("✗ Executable not found. Please build the project first:")
    print("  Windows: scripts/build.bat")
    print("  Linux:   scripts/build.sh")
    return None

def show_usage_examples():
    """Show various usage examples for the tool."""
    print_header("USAGE EXAMPLES")
    
    examples = [
        {
            "title": "System Monitoring Only",
            "command": "hardware-stress-tool --monitor-only --duration 10000",
            "description": "Monitor system metrics for 10 seconds without running stress tests"
        },
        {
            "title": "CPU Stress Test",
            "command": "hardware-stress-tool --cpu-test --intensity 7 --duration 30000",
            "description": "Run CPU stress test with high intensity for 30 seconds"
        },
        {
            "title": "Memory Stress Test",
            "command": "hardware-stress-tool --memory-test --intensity 5 --duration 20000",
            "description": "Run memory stress test with medium intensity for 20 seconds"
        },
        {
            "title": "Disk I/O Stress Test",
            "command": "hardware-stress-tool --disk-test --intensity 3 --duration 15000",
            "description": "Run disk I/O stress test with low intensity for 15 seconds"
        },
        {
            "title": "GPU Stress Test",
            "command": "hardware-stress-tool --gpu-test --intensity 6 --duration 25000",
            "description": "Run GPU stress test with medium-high intensity for 25 seconds"
        },
        {
            "title": "Multiple Tests",
            "command": "hardware-stress-tool --cpu-test --memory-test --intensity 4 --duration 45000",
            "description": "Run both CPU and memory stress tests for 45 seconds"
        },
        {
            "title": "All Tests with Fault Injection",
            "command": "hardware-stress-tool --fault-injection --intensity 5 --duration 60000",
            "description": "Run all stress tests with fault injection enabled for 1 minute"
        },
        {
            "title": "Custom Log File",
            "command": "hardware-stress-tool --cpu-test --log-file custom_test.log --duration 20000",
            "description": "Run CPU test and save logs to custom file"
        }
    ]
    
    for i, example in enumerate(examples, 1):
        print(f"\n{i}. {example['title']}")
        print(f"   Command: {example['command']}")
        print(f"   Description: {example['description']}")

def show_feature_overview():
    """Show an overview of the tool's features."""
    print_header("FEATURE OVERVIEW")
    
    features = [
        {
            "category": "Stress Tests",
            "items": [
                "CPU Stress Test - Multi-threaded CPU-intensive calculations",
                "Memory Stress Test - Dynamic memory allocation and access patterns",
                "Disk I/O Stress Test - File creation, writing, and verification",
                "GPU Stress Test - Matrix operations simulating GPU workloads"
            ]
        },
        {
            "category": "System Monitoring",
            "items": [
                "Real-time CPU usage monitoring (Windows PDH, Linux /proc/stat)",
                "Memory usage tracking (Windows GlobalMemoryStatusEx, Linux sysinfo)",
                "Temperature monitoring (Linux thermal zones, Windows estimation)",
                "GPU usage and temperature monitoring (platform-specific)",
                "Configurable monitoring intervals and callbacks"
            ]
        },
        {
            "category": "Fault Injection",
            "items": [
                "Memory corruption simulation",
                "CPU overload injection",
                "Disk I/O error simulation",
                "Network packet loss simulation",
                "Timing anomaly injection",
                "Process termination",
                "System call failure simulation",
                "Auto-recovery mechanisms"
            ]
        },
        {
            "category": "Logging & Reporting",
            "items": [
                "Multi-level logging (DEBUG, INFO, WARNING, ERROR)",
                "Thread-safe logging with timestamps",
                "Configurable log files and levels",
                "System metrics logging",
                "Test result reporting"
            ]
        },
        {
            "category": "Configuration",
            "items": [
                "Command-line argument parsing",
                "Configurable test intensity (1-10 scale)",
                "Adjustable test duration",
                "Individual or combined test execution",
                "Cross-platform support (Windows/Linux)"
            ]
        }
    ]
    
    for feature in features:
        print(f"\n{feature['category']}:")
        for item in feature['items']:
            print(f"  • {item}")

def show_architecture_overview():
    """Show the tool's architecture and design."""
    print_header("ARCHITECTURE OVERVIEW")
    
    print("""
The Hardware Stress Tool is built with a modular, extensible architecture:

┌─────────────────────────────────────────────────────────────┐
│                    Main Application                         │
│  (main.cpp) - Command line interface and orchestration     │
└─────────────────┬───────────────────────────────────────────┘
                  │
    ┌─────────────┼─────────────┐
    │             │             │
┌───▼───┐   ┌────▼────┐   ┌────▼────┐
│Logger │   │System   │   │Stress   │
│       │   │Monitor  │   │Tester   │
└───────┘   └─────────┘   └─────────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
         ┌────▼───┐   ┌────▼───┐   ┌────▼───┐
         │CPU     │   │Memory  │   │Disk    │
         │Stress  │   │Stress  │   │Stress  │
         └────────┘   └────────┘   └────────┘

Key Design Principles:
• Separation of Concerns - Each component has a specific responsibility
• Extensibility - Easy to add new stress tests and fault types
• Thread Safety - All components are designed for concurrent operation
• Platform Independence - Abstract interfaces with platform-specific implementations
• Error Handling - Comprehensive error handling and recovery mechanisms
""")

def show_test_results_analysis():
    """Show how to analyze test results."""
    print_header("TEST RESULTS ANALYSIS")
    
    print("""
When running stress tests, the tool provides comprehensive results:

1. Real-time Metrics:
   • CPU usage percentage
   • Memory usage percentage  
   • System temperature
   • GPU usage and temperature
   • Timestamps for all measurements

2. Test Results:
   • Test duration and status
   • Peak resource utilization
   • Baseline vs. stressed metrics
   • Error messages and warnings

3. Log Analysis:
   • Detailed execution logs
   • Performance bottlenecks
   • System stability indicators
   • Fault injection effects

4. Performance Indicators:
   • Resource saturation levels
   • Thermal throttling detection
   • Memory pressure indicators
   • I/O performance degradation

Example log output:
[2025-01-20 10:30:15.123] [INFO] Starting CPU stress test with 8 threads
[2025-01-20 10:30:15.124] [DEBUG] SYSTEM_METRICS CPU:85.2% MEM:45.1% TEMP:67.3°C
[2025-01-20 10:30:16.125] [DEBUG] SYSTEM_METRICS CPU:92.7% MEM:47.8% TEMP:71.2°C
[2025-01-20 10:30:17.126] [INFO] CPU stress test completed in 2000ms
""")

def show_safety_guidelines():
    """Show safety guidelines for using the tool."""
    print_header("SAFETY GUIDELINES")
    
    print("""
⚠️  IMPORTANT SAFETY CONSIDERATIONS:

1. System Stability:
   • Start with low intensity (1-3) and short durations
   • Monitor system temperature and performance
   • Stop tests immediately if system becomes unresponsive
   • Don't run on production systems without proper testing

2. Hardware Protection:
   • Ensure adequate cooling for CPU and GPU stress tests
   • Monitor temperature sensors during testing
   • Avoid running multiple high-intensity tests simultaneously
   • Be aware of thermal throttling and power limits

3. Data Safety:
   • Disk stress tests create temporary files - ensure sufficient space
   • Memory stress tests can cause system slowdown
   • Fault injection may affect running applications
   • Always backup important data before testing

4. Recommended Usage:
   • Use in controlled test environments
   • Start with system monitoring only
   • Gradually increase test intensity
   • Monitor system resources during testing
   • Have a plan to stop tests if needed (Ctrl+C)

5. Recovery:
   • Tests automatically clean up resources
   • Fault injection has auto-recovery mechanisms
   • System monitoring continues until explicitly stopped
   • Log files help diagnose any issues
""")

def main():
    """Main demonstration function."""
    print_header("HARDWARE STRESS TOOL - DEMONSTRATION")
    print("This script demonstrates the capabilities and usage of the Hardware Stress Tool.")
    
    # Check build status
    executable_path = check_build_status()
    
    # Show features and usage
    show_feature_overview()
    show_architecture_overview()
    show_usage_examples()
    show_test_results_analysis()
    show_safety_guidelines()
    
    print_header("NEXT STEPS")
    print("""
To get started with the Hardware Stress Tool:

1. Build the project:
   • Windows: scripts/build.bat
   • Linux:   scripts/build.sh

2. Run the foundation test:
   • python scripts/test_foundation.py

3. Try basic system monitoring:
   • hardware-stress-tool --monitor-only --duration 5000

4. Run a simple stress test:
   • hardware-stress-tool --cpu-test --intensity 3 --duration 10000

5. Check the logs:
   • Review stress_test.log for detailed information

For more information, see the README.md file.
""")
    
    if executable_path:
        print(f"\n✅ Tool is ready to use: {executable_path}")
    else:
        print("\n⚠️  Please build the project before running tests")

if __name__ == "__main__":
    main() 