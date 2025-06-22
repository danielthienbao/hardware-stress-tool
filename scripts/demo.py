#!/usr/bin/env python3
"""
Hardware Stress Diagnostic Tool - Demo Simulator
This script demonstrates what the C++ tool does without requiring compilation.
"""

import time
import random
import threading
import psutil
import os
import sys

class HardwareStressDemo:
    def __init__(self):
        self.running = False
        
    def print_banner(self):
        print("=" * 50)
        print("    Hardware Stress Diagnostic Tool - DEMO")
        print("=" * 50)
        print("This Python demo simulates the C++ implementation")
        print("Cross-platform hardware stress testing utility\n")
    
    def get_system_info(self):
        print("System Information:")
        print("=" * 19)
        print(f"CPU Cores: {psutil.cpu_count()}")
        print(f"CPU Physical Cores: {psutil.cpu_count(logical=False)}")
        
        memory = psutil.virtual_memory()
        print(f"Total RAM: {memory.total // (1024*1024)} MB")
        print(f"Available RAM: {memory.available // (1024*1024)} MB")
        
        disk = psutil.disk_usage('/')
        print(f"Total Disk Space: {disk.total // (1024*1024*1024)} GB")
        print(f"Free Disk Space: {disk.free // (1024*1024*1024)} GB")
        print()
    
    def simulate_cpu_stress(self, duration=10):
        print(f"Simulating CPU Stress Test ({duration}s)...")
        print("Progress: [", end="", flush=True)
        
        start_time = time.time()
        operations = 0
        
        def cpu_worker():
            nonlocal operations
            while self.running:
                # Simulate CPU-intensive work
                sum(i*i for i in range(1000))
                operations += 1
                time.sleep(0.001)
        
        # Start worker threads
        self.running = True
        threads = []
        for _ in range(psutil.cpu_count()):
            t = threading.Thread(target=cpu_worker)
            t.start()
            threads.append(t)
        
        # Progress display
        for i in range(20):
            time.sleep(duration / 20)
            print("=", end="", flush=True)
        
        self.running = False
        for t in threads:
            t.join()
        
        print("]")
        print(f"CPU stress test completed!")
        print(f"Operations performed: {operations}")
        print(f"Threads used: {len(threads)}")
        print()
    
    def simulate_memory_stress(self, duration=10):
        print(f"Simulating Memory Stress Test ({duration}s)...")
        print("Progress: [", end="", flush=True)
        
        memory_blocks = []
        block_size = 10 * 1024 * 1024  # 10 MB blocks
        
        for i in range(20):
            try:
                # Allocate memory block
                block = bytearray(block_size)
                # Fill with random data
                for j in range(0, len(block), 1024):
                    block[j] = random.randint(0, 255)
                memory_blocks.append(block)
                
                print("=", end="", flush=True)
                time.sleep(duration / 20)
                
            except MemoryError:
                print("\n⚠ Memory allocation limit reached")
                break
        
        # Clean up
        del memory_blocks
        
        print("]")
        print(f"Memory stress test completed!")
        print()
    
    def simulate_disk_stress(self, duration=10):
        print(f"Simulating Disk Stress Test ({duration}s)...")
        print("Progress: [", end="", flush=True)
        
        test_file = "stress_test_temp.dat"
        data = b"A" * (1024 * 1024)  # 1 MB of data
        operations = 0
        
        start_time = time.time()
        while time.time() - start_time < duration:
            try:
                # Write test
                with open(test_file, "wb") as f:
                    f.write(data)
                    f.flush()
                    os.fsync(f.fileno())
                
                # Read test
                with open(test_file, "rb") as f:
                    read_data = f.read()
                    if read_data != data:
                        print("\n⚠ Data integrity error!")
                
                operations += 1
                
                # Progress display
                elapsed = time.time() - start_time
                progress = int((elapsed / duration) * 20)
                print("\r" + "Progress: [" + "=" * progress + " " * (20 - progress) + "]", end="", flush=True)
                
            except IOError as e:
                print(f"\n⚠ I/O Error: {e}")
                break
        
        # Clean up
        try:
            os.remove(test_file)
        except:
            pass
        
        print("]")
        print(f"Disk stress test completed!")
        print(f"I/O operations: {operations}")
        print()
    
    def simulate_system_monitoring(self, duration=15):
        print(f"Simulating System Monitoring ({duration}s)...")
        print("Real-time metrics (updating every second):\n")
        
        start_time = time.time()
        while time.time() - start_time < duration:
            # Get current system metrics
            cpu_percent = psutil.cpu_percent(interval=1)
            memory = psutil.virtual_memory()
            disk = psutil.disk_usage('/')
            
            # Display metrics
            print(f"\rCPU: {cpu_percent:5.1f}%  " +
                  f"Memory: {memory.percent:5.1f}%  " +
                  f"Disk: {(disk.used/disk.total)*100:5.1f}%  " +
                  f"RAM: {memory.used//(1024*1024):5d}MB used", end="", flush=True)
        
        print("\n\nSystem monitoring completed!")
        print()
    
    def simulate_fault_injection(self):
        print("Simulating Fault Injection Demo...")
        print("Demonstrating fault injection capabilities:\n")
        
        fault_types = [
            ("Random Delay", "Injecting 100-500ms delays"),
            ("Resource Exhaustion", "Simulating temporary resource shortage"),
            ("Exception Injection", "Throwing recoverable exceptions"),
            ("Memory Corruption", "Detecting memory integrity issues"),
            ("Thread Deadlock", "Simulating deadlock detection and recovery"),
            ("Disk I/O Failure", "Simulating storage device failures"),
            ("Network Timeout", "Simulating network connectivity issues")
        ]
        
        for fault_name, description in fault_types:
            print(f"Injecting fault: {fault_name}")
            print(f"  Description: {description}")
            
            # Simulate fault injection
            delay = random.uniform(0.1, 0.5)
            time.sleep(delay)
            
            # Simulate recovery
            recovery_success = random.choice([True, True, True, False])  # 75% success rate
            print(f"  Recovery: {'SUCCESS' if recovery_success else 'FAILED'}")
            print(f"  Recovery time: {delay*1000:.0f}ms\n")
            
            time.sleep(0.5)
        
        print("Fault injection demo completed!")
        print()
    
    def show_menu(self):
        print("Select test type:")
        print("1. CPU Stress Test")
        print("2. Memory Stress Test") 
        print("3. Disk Stress Test")
        print("4. System Monitor Demo")
        print("5. Fault Injection Demo")
        print("6. Run All Tests")
        print("0. Exit")
        return input("Choice: ").strip()
    
    def run(self):
        self.print_banner()
        self.get_system_info()
        
        while True:
            choice = self.show_menu()
            print()
            
            if choice == "1":
                self.simulate_cpu_stress()
            elif choice == "2":
                self.simulate_memory_stress()
            elif choice == "3":
                self.simulate_disk_stress()
            elif choice == "4":
                self.simulate_system_monitoring()
            elif choice == "5":
                self.simulate_fault_injection()
            elif choice == "6":
                print("Running all tests...")
                self.simulate_cpu_stress(5)
                self.simulate_memory_stress(5)
                self.simulate_disk_stress(5)
                self.simulate_system_monitoring(10)
                self.simulate_fault_injection()
            elif choice == "0":
                print("Exiting demo...")
                break
            else:
                print("Invalid choice. Please try again.\n")
            
            input("Press Enter to continue...")
            print()

def main():
    try:
        demo = HardwareStressDemo()
        demo.run()
    except KeyboardInterrupt:
        print("\n\nDemo interrupted by user.")
    except Exception as e:
        print(f"\nError: {e}")
        print("Note: This demo requires the 'psutil' package.")
        print("Install it with: pip install psutil")

if __name__ == "__main__":
    main() 