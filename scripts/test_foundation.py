#!/usr/bin/env python3
"""
Test script to demonstrate Hardware Stress Diagnostic Tool foundation
"""

import time
import random
import threading
from datetime import datetime

class MockLogger:
    def __init__(self, log_file="test_stress.log"):
        self.log_file = log_file
        
    def log(self, level, message):
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        log_entry = f"[{timestamp}] [{level}] {message}"
        print(log_entry)
        
        with open(self.log_file, 'a') as f:
            f.write(log_entry + '\n')
    
    def info(self, message):
        self.log("INFO", message)
    
    def debug(self, message):
        self.log("DEBUG", message)

class MockSystemMonitor:
    def __init__(self, logger):
        self.logger = logger
        self.running = False
        self.monitoring_thread = None
        
    def start_monitoring(self):
        if self.running:
            return
            
        self.running = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
        self.logger.info("System monitoring started")
    
    def stop_monitoring(self):
        self.running = False
        if self.monitoring_thread:
            self.monitoring_thread.join()
        self.logger.info("System monitoring stopped")
    
    def _monitoring_loop(self):
        while self.running:
            cpu_usage = random.uniform(10.0, 90.0)
            memory_usage = random.uniform(20.0, 80.0)
            temperature = random.uniform(30.0, 70.0)
            
            self.logger.debug(f"SYSTEM_METRICS CPU:{cpu_usage:.1f}% MEM:{memory_usage:.1f}% TEMP:{temperature:.1f}°C")
            time.sleep(1)

def main():
    print("🔧 Hardware Stress Diagnostic Tool - Foundation Test")
    print("=" * 60)
    
    # Initialize components
    logger = MockLogger("foundation_test.log")
    system_monitor = MockSystemMonitor(logger)
    
    logger.info("Hardware Stress Diagnostic Tool starting...")
    logger.info("All components initialized successfully")
    
    # Start system monitoring
    system_monitor.start_monitoring()
    
    # Run for 10 seconds
    time.sleep(10)
    
    # Stop monitoring
    system_monitor.stop_monitoring()
    
    logger.info("Hardware Stress Diagnostic Tool finished")
    
    print("\n" + "=" * 60)
    print("✅ Foundation test completed successfully!")
    print("📝 Check 'foundation_test.log' for detailed output")

if __name__ == "__main__":
    main() 