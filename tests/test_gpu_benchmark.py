"""
Unit tests for GPU benchmark module
Tests device performance validation
"""

import unittest
import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

try:
    from gpu_benchmark import GPUBenchmark
    import pycuda.autoinit
    HAS_PYCUDA = True
except ImportError:
    HAS_PYCUDA = False


@unittest.skipIf(not HAS_PYCUDA, "PyCUDA not available")
class TestGPUBenchmark(unittest.TestCase):
    """Unit tests for GPU benchmarking functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        if HAS_PYCUDA:
            try:
                self.benchmark = GPUBenchmark()
            except Exception as e:
                self.skipTest(f"Failed to initialize GPU benchmark: {e}")
    
    def test_memory_throughput_benchmark(self):
        """Test memory throughput benchmark returns valid results"""
        throughput = self.benchmark.benchmark_memory_throughput(size_mb=256, iterations=5)
        self.assertIsInstance(throughput, float)
        self.assertGreater(throughput, 0.0)
        self.assertLess(throughput, 10000.0)  # Reasonable upper bound in GB/s
    
    def test_compute_performance_benchmark(self):
        """Test compute performance benchmark returns valid GFLOPS"""
        gflops = self.benchmark.benchmark_compute_performance(matrix_size=256)
        self.assertIsInstance(gflops, float)
        self.assertGreater(gflops, 0.0)
        self.assertLess(gflops, 100000.0)  # Reasonable upper bound
    
    def test_concurrency_benchmark(self):
        """Test concurrency benchmark with multiple streams"""
        throughput = self.benchmark.benchmark_concurrency(num_streams=2, size_mb=128)
        self.assertIsInstance(throughput, float)
        self.assertGreater(throughput, 0.0)
    
    def test_thermal_stress_test(self):
        """Test thermal stress test returns performance samples"""
        samples = self.benchmark.thermal_stress_test(duration=5)
        self.assertIsInstance(samples, list)
        if samples:
            self.assertIn('time', samples[0])
            self.assertIn('iter_time', samples[0])
            self.assertIn('iteration', samples[0])
    
    def test_memory_throughput_scalability(self):
        """Test memory throughput scales with data size"""
        small_throughput = self.benchmark.benchmark_memory_throughput(size_mb=128, iterations=5)
        large_throughput = self.benchmark.benchmark_memory_throughput(size_mb=512, iterations=5)
        # Throughput should be similar (within 50% variance)
        ratio = large_throughput / small_throughput
        self.assertGreater(ratio, 0.5)
        self.assertLess(ratio, 2.0)
    
    def test_kernel_loading(self):
        """Test that all required kernels are loaded"""
        self.assertIsNotNone(self.benchmark.memory_mod)
        self.assertIsNotNone(self.benchmark.compute_mod)
        self.assertIsNotNone(self.benchmark.concurrency_mod)
        self.assertIsNotNone(self.benchmark.memory_copy)
        self.assertIsNotNone(self.benchmark.matrix_multiply)
        self.assertIsNotNone(self.benchmark.concurrent_stream)


if __name__ == '__main__':
    unittest.main()

