cmake_minimum_required(VERSION 3.16)
project(HardwareStressTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Header files
set(HEADERS
    include/stress_tester.h
    include/system_monitor.h
    include/logger.h
    include/fault_injector.h
)

# Create executable
add_executable(stress_tool ${SOURCES} ${HEADERS})

# Platform-specific libraries and compiler flags
if(WIN32)
    target_link_libraries(stress_tool PRIVATE ws2_32 psapi)
    target_compile_definitions(stress_tool PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(UNIX)
    target_link_libraries(stress_tool PRIVATE pthread)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(stress_tool PRIVATE /W4)
else()
    target_compile_options(stress_tool PRIVATE -Wall -Wextra -O2)
endif()

# Set output directory
set_target_properties(stress_tool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
) 