cmake_minimum_required(VERSION 3.16)
project(hardware-stress-tool VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/stress_tester.cpp
    src/system_monitor.cpp
    src/logger.cpp
    src/fault_injector.cpp
)

# Create executable
add_executable(hardware-stress-tool ${SOURCES})

# Link libraries
target_link_libraries(hardware-stress-tool Threads::Threads)

# Install target
install(TARGETS hardware-stress-tool DESTINATION bin)

# Test target
enable_testing()
add_executable(stress_tool_tests tests/test_stress_tester.cpp ${SOURCES})
target_link_libraries(stress_tool_tests Threads::Threads)
add_test(NAME StressToolTests COMMAND stress_tool_tests) 