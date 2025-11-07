"""
GPU Benchmarking Module using C++ CUDA kernels via PyCUDA
Tests GPU memory throughput, thermal efficiency, and concurrency
"""

import os
import time
import numpy as np
try:
    import pycuda.autoinit
    import pycuda.driver as cuda
    import pycuda.compiler
    from pycuda.compiler import SourceModule
    HAS_PYCUDA = True
except ImportError:
    HAS_PYCUDA = False

class GPUBenchmark:
    """GPU benchmarking using compiled C++ CUDA kernels"""
    
    def __init__(self):
        if not HAS_PYCUDA:
            raise RuntimeError("PyCUDA not available")
        
        self.build_dir = "build"
        self._load_kernels()
    
    def _load_kernels(self):
        """Load compiled C++ CUDA kernels from PTX files"""
        # Load memory throughput kernels
        memory_ptx_path = os.path.join(self.build_dir, "memory_throughput.ptx")
        if not os.path.exists(memory_ptx_path):
            raise FileNotFoundError(
                f"Kernel not found: {memory_ptx_path}. Run 'make' first to compile CUDA kernels."
            )
        with open(memory_ptx_path, 'r') as f:
            memory_ptx = f.read()
        self.memory_mod = SourceModule(memory_ptx)
        self.memory_copy = self.memory_mod.get_function("memory_copy_kernel")
        self.memory_bandwidth = self.memory_mod.get_function("memory_bandwidth_kernel")
        self.concurrent_memory = self.memory_mod.get_function("concurrent_memory_kernel")
        
        # Load compute-intensive kernels
        compute_ptx_path = os.path.join(self.build_dir, "compute_intensive.ptx")
        if not os.path.exists(compute_ptx_path):
            raise FileNotFoundError(
                f"Kernel not found: {compute_ptx_path}. Run 'make' first to compile CUDA kernels."
            )
        with open(compute_ptx_path, 'r') as f:
            compute_ptx = f.read()
        self.compute_mod = SourceModule(compute_ptx)
        self.matrix_multiply = self.compute_mod.get_function("matrix_multiply_kernel")
        self.mandelbrot = self.compute_mod.get_function("mandelbrot_kernel")
        
        # Load concurrency kernels
        concurrency_ptx_path = os.path.join(self.build_dir, "concurrency.ptx")
        if not os.path.exists(concurrency_ptx_path):
            raise FileNotFoundError(
                f"Kernel not found: {concurrency_ptx_path}. Run 'make' first to compile CUDA kernels."
            )
        with open(concurrency_ptx_path, 'r') as f:
            concurrency_ptx = f.read()
        self.concurrency_mod = SourceModule(concurrency_ptx)
        self.concurrent_stream = self.concurrency_mod.get_function("concurrent_stream_kernel")
    
    def benchmark_memory_throughput(self, size_mb=1024, iterations=100):
        """
        Benchmark GPU memory throughput using custom C++ kernel
        Returns: throughput in GB/s
        """
        n = size_mb * 1024 * 1024 // 4  # float32 elements
        block_size = 256
        grid_size = (n + block_size - 1) // block_size
        
        # Allocate host and device memory
        host_data = np.random.randn(n).astype(np.float32)
        device_src = cuda.mem_alloc(host_data.nbytes)
        device_dst = cuda.mem_alloc(host_data.nbytes)
        
        # Copy to device
        cuda.memcpy_htod(device_src, host_data)
        
        # Warmup
        self.memory_copy(device_dst, device_src, np.int32(n),
                        block=(block_size, 1, 1), grid=(grid_size, 1))
        cuda.Context.synchronize()
        
        # Benchmark
        start_time = time.time()
        for _ in range(iterations):
            self.memory_copy(device_dst, device_src, np.int32(n),
                            block=(block_size, 1, 1), grid=(grid_size, 1))
        cuda.Context.synchronize()
        elapsed = time.time() - start_time
        
        # Calculate throughput
        bytes_transferred = size_mb * 1024 * 1024 * iterations * 2  # read + write
        throughput_gb_s = (bytes_transferred / (1024**3)) / elapsed
        
        # Cleanup
        device_src.free()
        device_dst.free()
        
        return throughput_gb_s
    
    def benchmark_compute_performance(self, matrix_size=1024):
        """
        Benchmark compute performance using matrix multiplication kernel
        Returns: GFLOPS
        """
        M = N = K = matrix_size
        block_size = 16
        
        # Allocate matrices
        A = np.random.randn(M * K).astype(np.float32)
        B = np.random.randn(K * N).astype(np.float32)
        C = np.zeros(M * N, dtype=np.float32)
        
        A_gpu = cuda.mem_alloc(A.nbytes)
        B_gpu = cuda.mem_alloc(B.nbytes)
        C_gpu = cuda.mem_alloc(C.nbytes)
        
        cuda.memcpy_htod(A_gpu, A)
        cuda.memcpy_htod(B_gpu, B)
        
        grid_size = ((N + block_size - 1) // block_size,
                     (M + block_size - 1) // block_size)
        
        # Warmup
        self.matrix_multiply(A_gpu, B_gpu, C_gpu,
                            np.int32(M), np.int32(N), np.int32(K),
                            block=(block_size, block_size, 1), grid=grid_size)
        cuda.Context.synchronize()
        
        # Benchmark
        iterations = 10
        start_time = time.time()
        for _ in range(iterations):
            self.matrix_multiply(A_gpu, B_gpu, C_gpu,
                                np.int32(M), np.int32(N), np.int32(K),
                                block=(block_size, block_size, 1), grid=grid_size)
        cuda.Context.synchronize()
        elapsed = time.time() - start_time
        
        # Calculate GFLOPS: 2*M*N*K operations per matrix multiply
        flops = 2 * M * N * K * iterations
        gflops = (flops / 1e9) / elapsed
        
        A_gpu.free()
        B_gpu.free()
        C_gpu.free()
        
        return gflops
    
    def benchmark_concurrency(self, num_streams=4, size_mb=512):
        """
        Benchmark GPU concurrency using multiple streams
        Returns: effective throughput in GB/s
        """
        n = size_mb * 1024 * 1024 // 4
        block_size = 256
        grid_size = (n + block_size - 1) // block_size
        
        # Create streams
        streams = [cuda.Stream() for _ in range(num_streams)]
        
        # Allocate data for each stream
        data_arrays = []
        for i in range(num_streams):
            host_data = np.random.randn(n).astype(np.float32)
            device_data = cuda.mem_alloc(host_data.nbytes)
            cuda.memcpy_htod_async(device_data, host_data, stream=streams[i])
            data_arrays.append(device_data)
        
        # Launch concurrent kernels
        start_time = time.time()
        for i, stream in enumerate(streams):
            self.concurrent_stream(data_arrays[i], np.int32(n), np.int32(i),
                                  block=(block_size, 1, 1), grid=(grid_size, 1),
                                  stream=stream)
        
        # Synchronize all streams
        for stream in streams:
            stream.synchronize()
        elapsed = time.time() - start_time
        
        # Calculate effective throughput
        bytes_per_stream = size_mb * 1024 * 1024
        total_bytes = bytes_per_stream * num_streams
        throughput_gb_s = (total_bytes / (1024**3)) / elapsed
        
        # Cleanup
        for data in data_arrays:
            data.free()
        
        return throughput_gb_s
    
    def thermal_stress_test(self, duration=60):
        """
        Thermal stress test using compute-intensive Mandelbrot kernel
        Monitors performance degradation over time
        """
        width, height = 2048, 2048
        max_iter = 1000
        block_size = 16
        
        output = np.zeros(width * height, dtype=np.float32)
        output_gpu = cuda.mem_alloc(output.nbytes)
        
        grid_size = ((width + block_size - 1) // block_size,
                     (height + block_size - 1) // block_size)
        
        start_time = time.time()
        iteration = 0
        performance_samples = []
        
        while time.time() - start_time < duration:
            iter_start = time.time()
            self.mandelbrot(output_gpu, np.int32(width), np.int32(height),
                           np.int32(max_iter),
                           block=(block_size, block_size, 1), grid=grid_size)
            cuda.Context.synchronize()
            iter_time = time.time() - iter_start
            
            iteration += 1
            if iteration % 10 == 0:
                performance_samples.append({
                    'time': time.time() - start_time,
                    'iter_time': iter_time,
                    'iteration': iteration
                })
        
        output_gpu.free()
        return performance_samples

