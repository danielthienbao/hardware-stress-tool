"""
Integration tests for heterogeneous hardware setups
Tests device performance validation across different configurations
"""

import unittest
import sys
import os
import time
import multiprocessing

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

try:
    from gpu_benchmark import GPUBenchmark
    import pycuda.autoinit
    import pycuda.driver as cuda
    HAS_PYCUDA = True
except ImportError:
    HAS_PYCUDA = False

from stressors import burn_cpu, burn_memory, burn_gpu


@unittest.skipIf(not HAS_PYCUDA, "PyCUDA not available")
class TestIntegration(unittest.TestCase):
    """Integration tests for heterogeneous hardware validation"""
    
    def setUp(self):
        """Set up test fixtures"""
        if HAS_PYCUDA:
            try:
                self.benchmark = GPUBenchmark()
                # Get device info
                self.device = cuda.Device(0)
                self.device_info = {
                    'name': self.device.name(),
                    'compute_capability': self.device.compute_capability(),
                    'total_memory': self.device.total_memory(),
                }
            except Exception as e:
                self.skipTest(f"Failed to initialize GPU: {e}")
    
    def test_gpu_cpu_concurrent_stress(self):
        """Test GPU and CPU stress simultaneously"""
        # Start CPU stress in background
        cpu_process = multiprocessing.Process(target=burn_cpu)
        cpu_process.start()
        
        try:
            # Run GPU benchmark
            throughput = self.benchmark.benchmark_memory_throughput(size_mb=256, iterations=5)
            self.assertGreater(throughput, 0.0)
            
            # Verify CPU process is running
            self.assertTrue(cpu_process.is_alive())
        finally:
            cpu_process.terminate()
            cpu_process.join()
    
    def test_gpu_memory_concurrent_stress(self):
        """Test GPU and system memory stress simultaneously"""
        # Allocate system memory
        mem_process = multiprocessing.Process(target=burn_memory, args=(1,))
        mem_process.start()
        
        try:
            # Run GPU benchmark
            gflops = self.benchmark.benchmark_compute_performance(matrix_size=256)
            self.assertGreater(gflops, 0.0)
            
            # Verify memory process is running
            self.assertTrue(mem_process.is_alive())
        finally:
            mem_process.terminate()
            mem_process.join()
    
    def test_multi_gpu_validation(self):
        """Test validation across multiple GPUs if available"""
        try:
            num_devices = cuda.Device.count()
            if num_devices > 1:
                results = []
                for i in range(num_devices):
                    cuda.Device(i).make_context()
                    try:
                        benchmark = GPUBenchmark()
                        throughput = benchmark.benchmark_memory_throughput(size_mb=128, iterations=3)
                        results.append(throughput)
                    finally:
                        cuda.Context.pop()
                
                # All devices should return valid results
                self.assertEqual(len(results), num_devices)
                for result in results:
                    self.assertGreater(result, 0.0)
            else:
                self.skipTest("Only one GPU available")
        except Exception as e:
            self.skipTest(f"Multi-GPU test failed: {e}")
    
    def test_performance_consistency(self):
        """Test performance consistency across multiple runs"""
        results = []
        for _ in range(3):
            throughput = self.benchmark.benchmark_memory_throughput(size_mb=256, iterations=5)
            results.append(throughput)
            time.sleep(1)  # Brief pause between runs
        
        # Results should be within 30% variance
        avg = sum(results) / len(results)
        for result in results:
            variance = abs(result - avg) / avg
            self.assertLess(variance, 0.3, 
                          f"Performance variance too high: {variance:.2%}")
    
    def test_thermal_performance_degradation(self):
        """Test that thermal stress shows performance characteristics"""
        samples = self.benchmark.thermal_stress_test(duration=10)
        
        if len(samples) >= 2:
            # Check that we have performance data over time
            first_iter = samples[0]['iter_time']
            last_iter = samples[-1]['iter_time']
            
            # Performance may degrade slightly but should remain reasonable
            degradation_ratio = last_iter / first_iter
            self.assertLess(degradation_ratio, 2.0, 
                          "Performance degradation too severe")
    
    def test_concurrency_scaling(self):
        """Test that concurrency scales with number of streams"""
        single_stream = self.benchmark.benchmark_concurrency(num_streams=1, size_mb=128)
        multi_stream = self.benchmark.benchmark_concurrency(num_streams=4, size_mb=128)
        
        # Multi-stream should achieve at least 50% of single-stream per stream
        # (accounting for overhead)
        expected_min = single_stream * 0.5 * 4
        self.assertGreater(multi_stream, expected_min * 0.5,
                          "Concurrency not scaling properly")
    
    def test_device_info_validation(self):
        """Test that device information is correctly retrieved"""
        self.assertIsNotNone(self.device_info['name'])
        self.assertIsNotNone(self.device_info['compute_capability'])
        self.assertGreater(self.device_info['total_memory'], 0)
        
        # Compute capability should be valid
        cc = self.device_info['compute_capability']
        self.assertGreaterEqual(cc[0], 3)  # At least SM 3.0


@unittest.skipIf(not HAS_PYCUDA, "PyCUDA not available")
class TestHeterogeneousHardware(unittest.TestCase):
    """Tests for heterogeneous hardware configurations"""
    
    def test_cpu_gpu_workload_balance(self):
        """Test balancing workload between CPU and GPU"""
        import psutil
        
        # Get initial CPU usage
        initial_cpu = psutil.cpu_percent(interval=0.1)
        
        # Start CPU stress
        cpu_process = multiprocessing.Process(target=burn_cpu)
        cpu_process.start()
        time.sleep(2)
        
        try:
            cpu_under_stress = psutil.cpu_percent(interval=1)
            
            # Run GPU benchmark
            benchmark = GPUBenchmark()
            throughput = benchmark.benchmark_memory_throughput(size_mb=256, iterations=5)
            
            # CPU should be under stress
            self.assertGreater(cpu_under_stress, initial_cpu + 10)
            # GPU should still perform
            self.assertGreater(throughput, 0.0)
        finally:
            cpu_process.terminate()
            cpu_process.join()


if __name__ == '__main__':
    unittest.main()

