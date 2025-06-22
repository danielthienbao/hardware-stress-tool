# Hardware Stress Diagnostic Tool Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Iinclude
LIBS = -pthread
TARGET = stress_tool
BUILDDIR = build
SRCDIR = src
INCDIR = include

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Default target
all: $(BUILDDIR)/$(TARGET)

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Build executable
$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)
	@echo "Build successful! Executable: $(BUILDDIR)/$(TARGET)"

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILDDIR)

# Install (copy to /usr/local/bin)
install: $(BUILDDIR)/$(TARGET)
	sudo cp $(BUILDDIR)/$(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(BUILDDIR)/$(TARGET)

# Build test executable
test-build: $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/test_basic.cpp $(filter-out $(BUILDDIR)/main.o, $(OBJECTS)) -o $(BUILDDIR)/test_runner $(LIBS)

# Run unit tests
test: test-build
	@echo "Running unit tests..."
	$(BUILDDIR)/test_runner

# Run all tests (unit tests + demo)
test-all: test
	@echo "Running test suite..."
	python3 scripts/run_tests.py

# Show help
help:
	@echo "Hardware Stress Diagnostic Tool - Makefile"
	@echo "=========================================="
	@echo "Available targets:"
	@echo "  all      - Build the executable (default)"
	@echo "  clean    - Remove build files"
	@echo "  debug    - Build with debug symbols"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  test     - Run basic functionality test"
	@echo "  help     - Show this help message"

.PHONY: all clean install uninstall debug test help 