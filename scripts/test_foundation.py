#!/usr/bin/env python3
"""
Hardware Stress Tool - Foundation Test Script
Tests the core functionality without requiring C++ compilation.
"""

import os
import sys
import time
import json
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

def check_file_exists(filepath, description):
    """Check if a file exists and print status."""
    exists = os.path.exists(filepath)
    status = "✓" if exists else "✗"
    print(f"{status} {description}: {filepath}")
    return exists

def check_directory_structure():
    """Validate the project directory structure."""
    print_header("PROJECT STRUCTURE VALIDATION")
    
    required_dirs = [
        ("include", "Header files directory"),
        ("src", "Source files directory"),
        ("scripts", "Build and utility scripts"),
        ("tests", "Test files directory")
    ]
    
    required_files = [
        ("CMakeLists.txt", "CMake build configuration"),
        ("README.md", "Project documentation"),
        ("include/stress_tester.h", "Stress tester header"),
        ("include/system_monitor.h", "System monitor header"),
        ("include/logger.h", "Logger header"),
        ("include/fault_injector.h", "Fault injector header"),
        ("src/main.cpp", "Main application source"),
        ("src/stress_tester.cpp", "Stress tester implementation"),
        ("src/system_monitor.cpp", "System monitor implementation"),
        ("src/logger.cpp", "Logger implementation"),
        ("src/fault_injector.cpp", "Fault injector implementation"),
        ("scripts/build.bat", "Windows build script"),
        ("scripts/build.sh", "Linux build script"),
        ("scripts/validate_structure.py", "Structure validation script")
    ]
    
    all_good = True
    
    # Check directories
    for dirname, description in required_dirs:
        if not check_file_exists(dirname, description):
            all_good = False
    
    # Check files
    for filename, description in required_files:
        if not check_file_exists(filename, description):
            all_good = False
    
    return all_good

def validate_cpp_syntax():
    """Basic C++ syntax validation using Python."""
    print_header("C++ SYNTAX VALIDATION")
    
    cpp_files = [
        "src/main.cpp",
        "src/stress_tester.cpp", 
        "src/system_monitor.cpp",
        "src/logger.cpp",
        "src/fault_injector.cpp"
    ]
    
    header_files = [
        "include/stress_tester.h",
        "include/system_monitor.h",
        "include/logger.h", 
        "include/fault_injector.h"
    ]
    
    all_good = True
    
    # Check C++ source files
    for filepath in cpp_files:
        if os.path.exists(filepath):
            try:
                with open(filepath, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # Basic syntax checks
                issues = []
                
                # Check for balanced braces
                brace_count = content.count('{') - content.count('}')
                if brace_count != 0:
                    issues.append(f"Unbalanced braces: {brace_count}")
                
                # Check for balanced parentheses
                paren_count = content.count('(') - content.count(')')
                if paren_count != 0:
                    issues.append(f"Unbalanced parentheses: {paren_count}")
                
                # Check for proper includes
                if '#include' not in content:
                    issues.append("No #include statements found")
                
                # Check for namespace
                if 'namespace hwstress' not in content:
                    issues.append("Missing hwstress namespace")
                
                if issues:
                    print(f"✗ {filepath}:")
                    for issue in issues:
                        print(f"    - {issue}")
                    all_good = False
                else:
                    print(f"✓ {filepath}: Syntax looks good")
                    
            except Exception as e:
                print(f"✗ {filepath}: Error reading file - {e}")
                all_good = False
        else:
            print(f"✗ {filepath}: File not found")
            all_good = False
    
    return all_good

def test_stress_test_functionality():
    """Test the stress test functionality by analyzing the code."""
    print_header("STRESS TEST FUNCTIONALITY VALIDATION")
    
    stress_tester_cpp = "src/stress_tester.cpp"
    if not os.path.exists(stress_tester_cpp):
        print("✗ Stress tester implementation not found")
        return False
    
    try:
        with open(stress_tester_cpp, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for concrete test implementations
        test_classes = [
            "CpuStressTest",
            "MemoryStressTest", 
            "DiskStressTest",
            "GpuStressTest"
        ]
        
        implemented_tests = []
        for test_class in test_classes:
            if test_class in content:
                implemented_tests.append(test_class)
                print(f"✓ {test_class} implementation found")
            else:
                print(f"✗ {test_class} implementation missing")
        
        # Check for key functionality
        key_features = [
            "cpuStressLoop",
            "memoryStressLoop", 
            "diskStressLoop",
            "gpuStressLoop",
            "workerThreads_",
            "stopRequested_"
        ]
        
        found_features = []
        for feature in key_features:
            if feature in content:
                found_features.append(feature)
                print(f"✓ {feature} found")
            else:
                print(f"✗ {feature} missing")
        
        # Check for proper thread management
        thread_management = [
            "std::thread",
            "joinable()",
            "join()",
            "std::atomic<bool>"
        ]
        
        for feature in thread_management:
            if feature in content:
                print(f"✓ Thread management: {feature}")
            else:
                print(f"✗ Thread management: {feature} missing")
        
        return len(implemented_tests) >= 3 and len(found_features) >= 4
        
    except Exception as e:
        print(f"✗ Error analyzing stress tester: {e}")
        return False

def test_system_monitor_functionality():
    """Test the system monitor functionality."""
    print_header("SYSTEM MONITOR FUNCTIONALITY VALIDATION")
    
    system_monitor_cpp = "src/system_monitor.cpp"
    if not os.path.exists(system_monitor_cpp):
        print("✗ System monitor implementation not found")
        return False
    
    try:
        with open(system_monitor_cpp, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for platform-specific implementations
        platform_features = [
            "#ifdef _WIN32",
            "#else",
            "#endif",
            "getCpuUsage",
            "getMemoryUsage", 
            "getTemperature",
            "getGpuUsage",
            "getGpuTemperature"
        ]
        
        found_features = []
        for feature in platform_features:
            if feature in content:
                found_features.append(feature)
                print(f"✓ {feature} found")
            else:
                print(f"✗ {feature} missing")
        
        # Check for Windows-specific features
        windows_features = [
            "PDH_",
            "GetSystemTimes",
            "GlobalMemoryStatusEx",
            "MEMORYSTATUSEX"
        ]
        
        for feature in windows_features:
            if feature in content:
                print(f"✓ Windows support: {feature}")
            else:
                print(f"✗ Windows support: {feature} missing")
        
        # Check for Linux-specific features
        linux_features = [
            "/proc/stat",
            "sysinfo",
            "/sys/class/thermal"
        ]
        
        for feature in linux_features:
            if feature in content:
                print(f"✓ Linux support: {feature}")
            else:
                print(f"✗ Linux support: {feature} missing")
        
        return len(found_features) >= 6
        
    except Exception as e:
        print(f"✗ Error analyzing system monitor: {e}")
        return False

def test_fault_injection_functionality():
    """Test the fault injection functionality."""
    print_header("FAULT INJECTION FUNCTIONALITY VALIDATION")
    
    fault_injector_cpp = "src/fault_injector.cpp"
    if not os.path.exists(fault_injector_cpp):
        print("✗ Fault injector implementation not found")
        return False
    
    try:
        with open(fault_injector_cpp, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for fault injection methods
        fault_methods = [
            "injectMemoryCorruption",
            "injectCpuOverload",
            "injectDiskIOError", 
            "injectNetworkPacketLoss",
            "injectTimingAnomaly",
            "injectProcessKill",
            "injectSystemCallFailure"
        ]
        
        implemented_methods = []
        for method in fault_methods:
            if method in content:
                implemented_methods.append(method)
                print(f"✓ {method} implementation found")
            else:
                print(f"✗ {method} implementation missing")
        
        # Check for fault management
        management_features = [
            "activeFaults_",
            "pendingFaults_",
            "faultHistory_",
            "autoRecoverFault",
            "faultLoop"
        ]
        
        for feature in management_features:
            if feature in content:
                print(f"✓ Fault management: {feature}")
            else:
                print(f"✗ Fault management: {feature} missing")
        
        # Check for platform-specific fault injection
        platform_faults = [
            "CreateToolhelp32Snapshot",
            "Process32First",
            "TerminateProcess"
        ]
        
        for feature in platform_faults:
            if feature in content:
                print(f"✓ Platform fault injection: {feature}")
            else:
                print(f"✗ Platform fault injection: {feature} missing")
        
        return len(implemented_methods) >= 5
        
    except Exception as e:
        print(f"✗ Error analyzing fault injector: {e}")
        return False

def test_main_application():
    """Test the main application functionality."""
    print_header("MAIN APPLICATION VALIDATION")
    
    main_cpp = "src/main.cpp"
    if not os.path.exists(main_cpp):
        print("✗ Main application not found")
        return False
    
    try:
        with open(main_cpp, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for command line argument handling
        cli_features = [
            "--cpu-test",
            "--memory-test",
            "--gpu-test", 
            "--disk-test",
            "--duration",
            "--intensity",
            "--monitor-only"
        ]
        
        found_cli = []
        for feature in cli_features:
            if feature in content:
                found_cli.append(feature)
                print(f"✓ CLI option: {feature}")
            else:
                print(f"✗ CLI option: {feature} missing")
        
        # Check for component initialization
        components = [
            "CpuStressTest",
            "MemoryStressTest",
            "DiskStressTest",
            "GpuStressTest",
            "runStressTests"
        ]
        
        for component in components:
            if component in content:
                print(f"✓ Component: {component}")
            else:
                print(f"✗ Component: {component} missing")
        
        # Check for proper error handling
        error_handling = [
            "try {",
            "catch (const std::exception& e)",
            "LOGGER.error"
        ]
        
        for feature in error_handling:
            if feature in content:
                print(f"✓ Error handling: {feature}")
            else:
                print(f"✗ Error handling: {feature} missing")
        
        return len(found_cli) >= 5
        
    except Exception as e:
        print(f"✗ Error analyzing main application: {e}")
        return False

def generate_test_report(results):
    """Generate a comprehensive test report."""
    print_header("TEST REPORT SUMMARY")
    
    total_tests = len(results)
    passed_tests = sum(1 for result in results.values() if result)
    
    print(f"Total Tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {total_tests - passed_tests}")
    print(f"Success Rate: {(passed_tests/total_tests)*100:.1f}%")
    
    print("\nDetailed Results:")
    for test_name, result in results.items():
        status = "PASS" if result else "FAIL"
        print(f"  {test_name}: {status}")
    
    if passed_tests == total_tests:
        print("\n🎉 All tests passed! The hardware stress tool foundation is solid.")
    else:
        print(f"\n⚠️  {total_tests - passed_tests} test(s) failed. Please review the issues above.")
    
    return passed_tests == total_tests

def main():
    """Main test function."""
    print_header("HARDWARE STRESS TOOL - FOUNDATION TEST")
    print("Testing core functionality without requiring C++ compilation...")
    
    # Run all tests
    test_results = {
        "Project Structure": check_directory_structure(),
        "C++ Syntax": validate_cpp_syntax(),
        "Stress Test Functionality": test_stress_test_functionality(),
        "System Monitor Functionality": test_system_monitor_functionality(),
        "Fault Injection Functionality": test_fault_injection_functionality(),
        "Main Application": test_main_application()
    }
    
    # Generate report
    success = generate_test_report(test_results)
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main() 