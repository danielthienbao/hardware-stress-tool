/**
 * Compute-intensive kernels for thermal and performance testing
 * Optimized for high-throughput execution
 */

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>

/**
 * Matrix multiplication kernel - compute-intensive workload
 */
__global__ void matrix_multiply_kernel(
    const float* A, const float* B, float* C,
    int M, int N, int K) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (row < M && col < N) {
        float sum = 0.0f;
        for (int k = 0; k < K; ++k) {
            sum += A[row * K + k] * B[k * N + col];
        }
        C[row * N + col] = sum;
    }
}

/**
 * Vector dot product with reduction - optimized compute pattern
 */
__global__ void vector_dot_product_kernel(
    const float* a, const float* b, float* result, int n) {
    extern __shared__ float sdata[];
    
    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    sdata[tid] = (idx < n) ? a[idx] * b[idx] : 0.0f;
    __syncthreads();
    
    // Reduction
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }
    
    if (tid == 0) {
        atomicAdd(result, sdata[0]);
    }
}

/**
 * FFT-like computation - high arithmetic intensity
 */
__global__ void fft_like_kernel(float* real, float* imag, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        const float PI = 3.14159265358979323846f;
        float angle = 2.0f * PI * idx / n;
        float r = real[idx];
        float i = imag[idx];
        
        // Simulate FFT computation
        for (int iter = 0; iter < 100; ++iter) {
            float cos_val = cosf(angle);
            float sin_val = sinf(angle);
            float new_r = r * cos_val - i * sin_val;
            float new_i = r * sin_val + i * cos_val;
            r = new_r;
            i = new_i;
        }
        
        real[idx] = r;
        imag[idx] = i;
    }
}

/**
 * Mandelbrot-like iterative computation - thermal stress test
 */
__global__ void mandelbrot_kernel(float* output, int width, int height, int max_iter) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        float cx = (x / (float)width) * 3.5f - 2.5f;
        float cy = (y / (float)height) * 2.0f - 1.0f;
        
        float zx = 0.0f, zy = 0.0f;
        int iter = 0;
        
        while (zx * zx + zy * zy < 4.0f && iter < max_iter) {
            float tmp = zx * zx - zy * zy + cx;
            zy = 2.0f * zx * zy + cy;
            zx = tmp;
            iter++;
        }
        
        output[y * width + x] = (float)iter / max_iter;
    }
}

