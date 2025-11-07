/**
 * High-throughput GPU memory bandwidth test kernel
 * Tests memory copy throughput with various access patterns
 */

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

/**
 * Memory copy kernel - tests global memory bandwidth
 */
__global__ void memory_copy_kernel(float* dst, const float* src, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        dst[idx] = src[idx];
    }
}

/**
 * Memory copy with stride - tests memory coalescing
 */
__global__ void memory_copy_stride_kernel(float* dst, const float* src, int n, int stride) {
    int idx = (blockIdx.x * blockDim.x + threadIdx.x) * stride;
    if (idx < n) {
        dst[idx] = src[idx];
    }
}

/**
 * Memory bandwidth test with read-modify-write pattern
 */
__global__ void memory_bandwidth_kernel(float* data, int n, int iterations) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        float val = data[idx];
        for (int i = 0; i < iterations; ++i) {
            val = val * 1.000001f + 0.000001f;
        }
        data[idx] = val;
    }
}

/**
 * Concurrent memory access pattern - tests memory throughput under concurrency
 */
__global__ void concurrent_memory_kernel(float* data1, float* data2, float* data3, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        // Concurrent read from multiple arrays
        float v1 = data1[idx];
        float v2 = data2[idx];
        float v3 = data3[idx];
        
        // Compute and write back
        float result = (v1 + v2) * v3;
        data1[idx] = result;
        data2[idx] = result * 0.5f;
        data3[idx] = result * 0.25f;
    }
}

