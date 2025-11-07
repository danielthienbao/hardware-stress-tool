import time
import os
import tempfile
import requests
try:
    import pycuda.autoinit
    import pycuda.driver as cuda
    import pycuda.compiler
    import numpy as np
    HAS_PYCUDA = True
except ImportError:
    HAS_PYCUDA = False

def burn_cpu():
    while True:
        x = 123456 ** 2

def burn_memory(size_gb):
    blocks = []
    try:
        for _ in range(size_gb):
            blocks.append(bytearray(1024 * 1024 * 1024))  # 1 GB
        while True:
            time.sleep(1)
    except MemoryError:
        print("[!] MemoryError: Could not allocate requested memory.")
        while True:
            time.sleep(1)

def burn_disk(path, size_gb):
    try:
        with open(path, 'wb') as f:
            for _ in range(size_gb):
                f.write(b'0' * 1024 * 1024 * 1024)  # Write 1GB at a time
        while True:
            time.sleep(1)
    except Exception as e:
        print(f"[!] Disk stress error: {e}")
        while True:
            time.sleep(1)

def burn_network(url, duration):
    end_time = time.time() + duration
    while time.time() < end_time:
        try:
            requests.get(url)
        except Exception:
            pass

def burn_gpu(duration):
    """GPU stress test using C++ CUDA kernels"""
    if not HAS_PYCUDA:
        print("[!] pycuda not installed. GPU stress not available.")
        while True:
            time.sleep(1)
        return
    
    try:
        from gpu_benchmark import GPUBenchmark
        benchmark = GPUBenchmark()
        
        print("[GPU] Starting GPU stress test with C++ kernels...")
        print("[GPU] Running memory throughput benchmark...")
        throughput = benchmark.benchmark_memory_throughput(size_mb=1024, iterations=10)
        print(f"[GPU] Memory throughput: {throughput:.2f} GB/s")
        
        print("[GPU] Running compute-intensive workload...")
        end_time = time.time() + duration
        iteration = 0
        while time.time() < end_time:
            gflops = benchmark.benchmark_compute_performance(matrix_size=512)
            iteration += 1
            if iteration % 10 == 0:
                print(f"[GPU] Compute performance: {gflops:.2f} GFLOPS")
        
        print("[GPU] Running concurrency test...")
        concurrency_throughput = benchmark.benchmark_concurrency(num_streams=4, size_mb=512)
        print(f"[GPU] Concurrent throughput: {concurrency_throughput:.2f} GB/s")
        
    except Exception as e:
        print(f"[!] GPU stress error: {e}")
        # Fallback to simple kernel if C++ kernels not available
        kernel_code = """
        __global__ void burn(float *a) {
            int idx = threadIdx.x + blockIdx.x * blockDim.x;
            for (int i = 0; i < 1000000; ++i) {
                a[idx] = a[idx] * 1.000001f + 0.000001f;
            }
        }
        """
        mod = pycuda.compiler.SourceModule(kernel_code)
        burn = mod.get_function("burn")
        a = np.ones(1024*1024, dtype=np.float32)
        a_gpu = cuda.mem_alloc(a.nbytes)
        cuda.memcpy_htod(a_gpu, a)
        end_time = time.time() + duration
        while time.time() < end_time:
            burn(a_gpu, block=(256,1,1), grid=(4096,1))
        cuda.mem_free(a_gpu) 