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
    if not HAS_PYCUDA:
        print("[!] pycuda not installed. GPU stress not available.")
        while True:
            time.sleep(1)
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