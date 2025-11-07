/**
 * Concurrency and multi-stream kernels for parallel execution testing
 */

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

/**
 * Multi-stream concurrent kernel - tests GPU concurrency capabilities
 */
__global__ void concurrent_stream_kernel(float* data, int n, int stream_id) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        // Each stream performs different computation pattern
        float val = data[idx];
        for (int i = 0; i < 1000; ++i) {
            switch (stream_id % 4) {
                case 0:
                    val = val * 1.0001f + 0.0001f;
                    break;
                case 1:
                    val = val * 0.9999f - 0.0001f;
                    break;
                case 2:
                    val = val * val * 0.5f;
                    break;
                case 3:
                    val = sqrtf(val * val + 1.0f);
                    break;
            }
        }
        data[idx] = val;
    }
}

/**
 * Atomic operations kernel - tests concurrent memory access
 */
__global__ void atomic_operations_kernel(int* counter, float* results, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        int count = atomicAdd(counter, 1);
        results[idx] = (float)count * 0.001f;
    }
}

/**
 * Pipeline kernel - tests memory/compute overlap
 */
__global__ void pipeline_kernel(
    const float* input, float* output, int n,
    float* intermediate) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        // Stage 1: Load and compute
        float val = input[idx];
        val = val * 2.0f + 1.0f;
        intermediate[idx] = val;
        
        __syncthreads();
        
        // Stage 2: Further processing
        if (idx > 0 && idx < n - 1) {
            output[idx] = (intermediate[idx-1] + intermediate[idx] + intermediate[idx+1]) / 3.0f;
        }
    }
}

