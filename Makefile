# Makefile for compiling CUDA kernels
NVCC = nvcc
# Use compute capability 7.5 (Turing) as default, but allow override
COMPUTE_CAP ?= 75
NVCC_FLAGS = -O3 -arch=sm_$(COMPUTE_CAP) -shared -Xcompiler -fPIC
KERNEL_DIR = kernels
BUILD_DIR = build
PYTHON_DIR = .

# Kernel source files
MEMORY_KERNEL = $(KERNEL_DIR)/memory_throughput.cu
COMPUTE_KERNEL = $(KERNEL_DIR)/compute_intensive.cu
CONCURRENCY_KERNEL = $(KERNEL_DIR)/concurrency.cu

# Output PTX files (for PyCUDA)
MEMORY_PTX = $(BUILD_DIR)/memory_throughput.ptx
COMPUTE_PTX = $(BUILD_DIR)/compute_intensive.ptx
CONCURRENCY_PTX = $(BUILD_DIR)/concurrency.ptx

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile kernels to PTX format for PyCUDA
$(MEMORY_PTX): $(MEMORY_KERNEL) | $(BUILD_DIR)
	$(NVCC) $(NVCC_FLAGS) -ptx -o $@ $<

$(COMPUTE_PTX): $(COMPUTE_KERNEL) | $(BUILD_DIR)
	$(NVCC) $(NVCC_FLAGS) -ptx -o $@ $<

$(CONCURRENCY_PTX): $(CONCURRENCY_KERNEL) | $(BUILD_DIR)
	$(NVCC) $(NVCC_FLAGS) -ptx -o $@ $<

# Build all kernels
all: $(MEMORY_PTX) $(COMPUTE_PTX) $(CONCURRENCY_PTX)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f *.pyc __pycache__

.PHONY: all clean

