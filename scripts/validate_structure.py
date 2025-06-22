#!/usr/bin/env python3
"""
Hardware Stress Diagnostic Tool - Project Structure Validator
This script validates the project structure and checks for missing files.
"""

import os
import sys
from pathlib import Path

def check_file_exists(filepath, description):
    """Check if a file exists and report the result."""
    if os.path.exists(filepath):
        print(f"✓ {description}: {filepath}")
        return True
    else:
        print(f"✗ {description}: {filepath} (MISSING)")
        return False

def check_directory_exists(dirpath, description):
    """Check if a directory exists and report the result."""
    if os.path.exists(dirpath) and os.path.isdir(dirpath):
        print(f"✓ {description}: {dirpath}")
        return True
    else:
        print(f"✗ {description}: {dirpath} (MISSING)")
        return False

def validate_cpp_syntax(filepath):
    """Basic C++ syntax validation (simple checks)."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            
        # Basic syntax checks
        issues = []
        
        # Check for missing semicolons (simplified)
        lines = content.split('\n')
        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            if stripped and not stripped.startswith('//') and not stripped.startswith('/*'):
                # Check for common patterns that should end with semicolon
                if (stripped.endswith('}') and not stripped.endswith('};') and 
                    ('class' in stripped or 'struct' in stripped)):
                    issues.append(f"Line {i}: Possible missing semicolon after class/struct")
        
        # Check for balanced braces
        open_braces = content.count('{')
        close_braces = content.count('}')
        if open_braces != close_braces:
            issues.append(f"Unbalanced braces: {open_braces} opening, {close_braces} closing")
        
        return issues
    except Exception as e:
        return [f"Error reading file: {e}"]

def main():
    print("Hardware Stress Diagnostic Tool - Project Validator")
    print("=" * 55)
    
    # Get project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    print(f"Project root: {project_root}")
    print()
    
    # Check project structure
    print("Checking project structure...")
    print("-" * 30)
    
    all_good = True
    
    # Required directories
    directories = [
        ("include", "Header files directory"),
        ("src", "Source files directory"),
        ("scripts", "Build scripts directory"),
        ("build", "Build output directory (will be created during build)")
    ]
    
    for dirname, desc in directories:
        dirpath = project_root / dirname
        if dirname == "build":
            # Build directory is optional
            if not check_directory_exists(dirpath, desc):
                print(f"  → Build directory will be created during build process")
        else:
            if not check_directory_exists(dirpath, desc):
                all_good = False
    
    print()
    
    # Required files
    print("Checking required files...")
    print("-" * 25)
    
    files = [
        ("CMakeLists.txt", "CMake build configuration"),
        ("Makefile", "Unix Makefile"),
        ("scripts/build.bat", "Windows build script"),
        ("scripts/build.sh", "Unix build script"),
        ("include/logger.h", "Logger header"),
        ("include/stress_tester.h", "Stress tester header"),
        ("include/system_monitor.h", "System monitor header"),
        ("include/fault_injector.h", "Fault injector header"),
        ("src/main.cpp", "Main application"),
        ("src/logger.cpp", "Logger implementation"),
        ("src/stress_tester.cpp", "Stress tester implementation"),
        ("src/system_monitor.cpp", "System monitor implementation"),
        ("src/fault_injector.cpp", "Fault injector implementation"),
    ]
    
    for filename, desc in files:
        filepath = project_root / filename
        if not check_file_exists(filepath, desc):
            all_good = False
    
    print()
    
    # Basic syntax validation for C++ files
    print("Performing basic C++ syntax checks...")
    print("-" * 40)
    
    cpp_files = [
        "src/main.cpp",
        "src/logger.cpp", 
        "src/stress_tester.cpp",
        "src/system_monitor.cpp",
        "src/fault_injector.cpp"
    ]
    
    for cpp_file in cpp_files:
        filepath = project_root / cpp_file
        if os.path.exists(filepath):
            issues = validate_cpp_syntax(filepath)
            if issues:
                print(f"⚠ Issues in {cpp_file}:")
                for issue in issues:
                    print(f"  - {issue}")
                all_good = False
            else:
                print(f"✓ {cpp_file}: No obvious syntax issues")
    
    print()
    
    # Summary
    print("Validation Summary")
    print("-" * 18)
    
    if all_good:
        print("✓ All checks passed! Project structure looks good.")
        print()
        print("Next steps:")
        print("1. Install a C++ compiler (Visual Studio on Windows, g++ on Linux)")
        print("2. Optionally install CMake for easier building")
        print("3. Run the appropriate build script:")
        print("   - Windows: scripts\\build.bat")
        print("   - Linux:   scripts/build.sh")
        print("   - Or use:  make")
        return 0
    else:
        print("✗ Some issues found. Please fix the missing files/directories.")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 