#include "../include/logger.h"
#include "../include/system_monitor.h"
#include "../include/stress_tester.h"
#include "../include/fault_injector.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

// Simple test framework
class SimpleTest {
public:
    static int tests_run;
    static int tests_passed;
    
    static void run_test(const std::string& test_name, bool (*test_func)()) {
        std::cout << "Running " << test_name << "... ";
        tests_run++;
        
        try {
            if (test_func()) {
                std::cout << "PASSED" << std::endl;
                tests_passed++;
            } else {
                std::cout << "FAILED" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "FAILED (Exception: " << e.what() << ")" << std::endl;
        }
    }
    
    static void print_summary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Success rate: " << (100.0 * tests_passed / tests_run) << "%" << std::endl;
    }
};

int SimpleTest::tests_run = 0;
int SimpleTest::tests_passed = 0;

// Logger Tests
bool test_logger_basic() {
    Logger& logger = Logger::getInstance();
    logger.initialize("test.log");
    logger.log(LogLevel::INFO, "Test message");
    return true; // If we get here without crashing, test passed
}

bool test_logger_levels() {
    Logger& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::WARNING);
    
    // This should work (WARNING >= WARNING)
    logger.log(LogLevel::WARNING, "Warning message");
    logger.log(LogLevel::ERROR, "Error message");
    
    // This should be filtered out (DEBUG < WARNING)
    logger.log(LogLevel::DEBUG, "Debug message");
    
    return true;
}

// System Monitor Tests
bool test_system_monitor_creation() {
    SystemMonitor monitor;
    return true; // Just test that we can create one
}

bool test_system_monitor_system_info() {
    std::string info = SystemMonitor::getSystemInfo();
    return !info.empty(); // Should return some system info
}

bool test_system_monitor_core_count() {
    int cores = SystemMonitor::getCoreCount();
    return cores > 0; // Should have at least 1 core
}

bool test_system_monitor_metrics() {
    SystemMonitor monitor;
    SystemMetrics metrics = monitor.getCurrentMetrics();
    
    // Basic sanity checks
    return metrics.cpuUsage >= 0.0 && 
           metrics.memoryUsage >= 0.0 && 
           metrics.totalMemory > 0;
}

// Stress Tester Tests
bool test_stress_tester_creation() {
    StressTester tester;
    return !tester.isRunning(); // Should not be running initially
}

bool test_stress_tester_short_cpu_test() {
    StressTester tester;
    
    StressTestConfig config;
    config.type = StressType::CPU;
    config.durationSeconds = 1; // Very short test
    config.threadCount = 1;     // Single thread
    
    tester.startTest(config);
    
    // Wait for test to complete
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    StressTestResults results = tester.getResults();
    return results.operationsCompleted > 0;
}

bool test_stress_tester_config() {
    StressTestConfig config;
    config.type = StressType::MEMORY;
    config.durationSeconds = 5;
    config.memorySize = 1024 * 1024; // 1MB
    
    // Just test that we can create and configure
    return config.durationSeconds == 5;
}

// Fault Injector Tests
bool test_fault_injector_creation() {
    FaultInjector injector;
    return true; // Just test creation
}

bool test_fault_injector_enable_disable() {
    FaultInjector injector;
    
    FaultConfig config;
    config.probability = 0.0; // 0% chance for testing
    config.enableRecovery = true;
    
    injector.enableFaultInjection(config);
    injector.disableFaultInjection();
    
    return true; // If no crash, test passed
}

bool test_fault_injector_safe_faults() {
    FaultInjector injector;
    
    FaultConfig config;
    config.probability = 1.0; // 100% for testing
    config.enableRecovery = true;
    
    injector.enableFaultInjection(config);
    
    // Test safe fault types
    injector.injectFault(FaultType::RANDOM_DELAYS);
    
    std::vector<FaultInjectionResult> history = injector.getInjectionHistory();
    
    injector.disableFaultInjection();
    
    return history.size() > 0; // Should have recorded something
}

// Integration Tests
bool test_basic_integration() {
    // Test that all components can work together
    Logger& logger = Logger::getInstance();
    logger.initialize("integration_test.log");
    
    SystemMonitor monitor;
    StressTester tester;
    FaultInjector injector;
    
    // If we can create all components without crashing, that's good
    return true;
}

bool test_short_full_workflow() {
    // Test a mini version of the full workflow
    Logger& logger = Logger::getInstance();
    SystemMonitor monitor;
    StressTester tester;
    
    // Start monitoring
    monitor.startMonitoring(500); // 0.5 second intervals
    
    // Run a very short stress test
    StressTestConfig config;
    config.type = StressType::CPU;
    config.durationSeconds = 1;
    config.threadCount = 1;
    
    tester.startTest(config);
    
    // Wait for completion
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    monitor.stopMonitoring();
    
    StressTestResults results = tester.getResults();
    return results.operationsCompleted > 0;
}

int main() {
    std::cout << "Hardware Stress Diagnostic Tool - Basic Tests" << std::endl;
    std::cout << "==============================================" << std::endl << std::endl;
    
    // Logger Tests
    std::cout << "Testing Logger..." << std::endl;
    SimpleTest::run_test("Logger Basic", test_logger_basic);
    SimpleTest::run_test("Logger Levels", test_logger_levels);
    std::cout << std::endl;
    
    // System Monitor Tests
    std::cout << "Testing System Monitor..." << std::endl;
    SimpleTest::run_test("System Monitor Creation", test_system_monitor_creation);
    SimpleTest::run_test("System Info", test_system_monitor_system_info);
    SimpleTest::run_test("Core Count", test_system_monitor_core_count);
    SimpleTest::run_test("Basic Metrics", test_system_monitor_metrics);
    std::cout << std::endl;
    
    // Stress Tester Tests
    std::cout << "Testing Stress Tester..." << std::endl;
    SimpleTest::run_test("Stress Tester Creation", test_stress_tester_creation);
    SimpleTest::run_test("Configuration", test_stress_tester_config);
    SimpleTest::run_test("Short CPU Test", test_stress_tester_short_cpu_test);
    std::cout << std::endl;
    
    // Fault Injector Tests
    std::cout << "Testing Fault Injector..." << std::endl;
    SimpleTest::run_test("Fault Injector Creation", test_fault_injector_creation);
    SimpleTest::run_test("Enable/Disable", test_fault_injector_enable_disable);
    SimpleTest::run_test("Safe Faults", test_fault_injector_safe_faults);
    std::cout << std::endl;
    
    // Integration Tests
    std::cout << "Testing Integration..." << std::endl;
    SimpleTest::run_test("Basic Integration", test_basic_integration);
    SimpleTest::run_test("Short Workflow", test_short_full_workflow);
    std::cout << std::endl;
    
    SimpleTest::print_summary();
    
    // Clean up test files
    std::remove("test.log");
    std::remove("integration_test.log");
    
    return (SimpleTest::tests_passed == SimpleTest::tests_run) ? 0 : 1;
} 