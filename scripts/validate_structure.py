#!/usr/bin/env python3
"""
Validation script for Hardware Stress Diagnostic Tool
Checks project structure and validates C++ code files
"""

import os
import sys
import re
from pathlib import Path

def check_file_exists(filepath, description):
    """Check if a file exists and print status"""
    if os.path.exists(filepath):
        print(f"✅ {description}: {filepath}")
        return True
    else:
        print(f"❌ {description}: {filepath} (MISSING)")
        return False

def validate_cpp_file(filepath, description, is_header=True):
    """Validate a C++ file for basic structure"""
    if not check_file_exists(filepath, description):
        return False
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for basic C++ structure
        has_namespace = 'namespace hwstress' in content
        has_std_includes = '#include <' in content
        
        print(f"   - Namespace: {'✅' if has_namespace else '❌'}")
        
        if is_header:
            has_include_guards = '#pragma once' in content or '#ifndef' in content
            print(f"   - Include guards: {'✅' if has_include_guards else '❌'}")
            print(f"   - Standard includes: {'✅' if has_std_includes else '❌'}")
            return has_namespace and has_include_guards and has_std_includes
        else:
            print(f"   - Standard includes: {'✅' if has_std_includes else '❌'}")
            return has_namespace and has_std_includes
        
    except Exception as e:
        print(f"   - Error reading file: {e}")
        return False

def main():
    print("🔍 Hardware Stress Diagnostic Tool - Structure Validation")
    print("=" * 60)
    
    # Check project structure
    print("\n📁 Project Structure:")
    required_dirs = [
        ('include', 'Header files directory'),
        ('src', 'Source files directory'),
        ('scripts', 'Build scripts directory'),
        ('tests', 'Test files directory')
    ]
    
    for dirname, description in required_dirs:
        check_file_exists(dirname, description)
    
    # Check header files
    print("\n📋 Header Files:")
    headers = [
        ('include/logger.h', 'Logger header'),
        ('include/system_monitor.h', 'System monitor header'),
        ('include/stress_tester.h', 'Stress tester header'),
        ('include/fault_injector.h', 'Fault injector header')
    ]
    
    header_valid = True
    for header, description in headers:
        if not validate_cpp_file(header, description, is_header=True):
            header_valid = False
    
    # Check source files
    print("\n💻 Source Files:")
    sources = [
        ('src/main.cpp', 'Main entry point'),
        ('src/logger.cpp', 'Logger implementation'),
        ('src/system_monitor.cpp', 'System monitor implementation'),
        ('src/stress_tester.cpp', 'Stress tester implementation'),
        ('src/fault_injector.cpp', 'Fault injector implementation')
    ]
    
    source_valid = True
    for source, description in sources:
        if not validate_cpp_file(source, description, is_header=False):
            source_valid = False
    
    # Check build files
    print("\n🔨 Build Files:")
    build_files = [
        ('CMakeLists.txt', 'CMake configuration'),
        ('scripts/build.bat', 'Windows build script'),
        ('scripts/build.sh', 'Linux build script')
    ]
    
    for build_file, description in build_files:
        check_file_exists(build_file, description)
    
    # Check documentation
    print("\n📚 Documentation:")
    docs = [
        ('README.md', 'Project documentation'),
        ('LICENSE', 'License file')
    ]
    
    for doc, description in docs:
        check_file_exists(doc, description)
    
    # Summary
    print("\n" + "=" * 60)
    print("📊 Validation Summary:")
    
    if header_valid and source_valid:
        print("✅ All C++ files are properly structured")
        print("✅ Project foundation is ready for development")
        print("\n🚀 Next Steps:")
        print("1. Install CMake (3.16+) and a C++17 compiler")
        print("2. Run: scripts/build.bat (Windows) or scripts/build.sh (Linux)")
        print("3. Test with: ./hardware-stress-tool --monitor-only")
        print("4. Start implementing Phase 2 features")
    else:
        print("❌ Some C++ files have issues")
        print("🔧 Please fix the issues above before proceeding")
    
    print("\n📝 Development Phases:")
    print("Phase 1: Foundation ✅ (Current)")
    print("Phase 2: Core Stress Tests")
    print("Phase 3: Advanced Features")
    print("Phase 4: Production Features")

if __name__ == "__main__":
    main() 