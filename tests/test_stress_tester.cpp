#include "../include/stress_tester.h"
#include "../include/logger.h"
#include "../include/system_monitor.h"
#include "../include/fault_injector.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

using namespace hwstress;

void test_cpu_stress_test() {
    std::cout << "Testing CPU Stress Test..." << std::endl;
    
    auto test = std::make_unique<CpuStressTest>();
    
    // Test configuration
    TestConfig config;
    config.type = TestType::CPU_STRESS;
    config.name = "Test CPU Stress";
    config.duration = std::chrono::milliseconds(1000); // 1 second
    config.intensity = 3;
    config.monitorMetrics = true;
    
    test->setConfig(config);
    
    // Start test
    test->start();
    assert(test->isRunning());
    
    // Let it run for a short time
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Stop test
    test->stop();
    assert(!test->isRunning());
    
    // Check result
    auto result = test->getResult();
    assert(result.status == TestStatus::COMPLETED);
    assert(result.duration.count() > 0);
    
    std::cout << "✓ CPU Stress Test passed" << std::endl;
}

void test_memory_stress_test() {
    std::cout << "Testing Memory Stress Test..." << std::endl;
    
    auto test = std::make_unique<MemoryStressTest>();
    
    // Test configuration
    TestConfig config;
    config.type = TestType::MEMORY_STRESS;
    config.name = "Test Memory Stress";
    config.duration = std::chrono::milliseconds(1000);
    config.intensity = 2;
    config.monitorMetrics = true;
    
    test->setConfig(config);
    
    // Start test
    test->start();
    assert(test->isRunning());
    
    // Let it run for a short time
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Stop test
    test->stop();
    assert(!test->isRunning());
    
    // Check result
    auto result = test->getResult();
    assert(result.status == TestStatus::COMPLETED);
    
    std::cout << "✓ Memory Stress Test passed" << std::endl;
}

void test_stress_tester_integration() {
    std::cout << "Testing Stress Tester Integration..." << std::endl;
    
    auto tester = std::make_unique<StressTester>();
    
    // Add tests
    tester->addTest(std::make_unique<CpuStressTest>());
    tester->addTest(std::make_unique<MemoryStressTest>());
    tester->addTest(std::make_unique<DiskStressTest>());
    tester->addTest(std::make_unique<GpuStressTest>());
    
    // Set global configuration
    tester->setGlobalDuration(std::chrono::milliseconds(500));
    tester->setGlobalIntensity(2);
    
    // Set up callbacks
    int testStartCount = 0;
    int testCompleteCount = 0;
    
    tester->setTestStartCallback([&testStartCount](const std::string& testName) {
        testStartCount++;
        std::cout << "  Started: " << testName << std::endl;
    });
    
    tester->setTestCompleteCallback([&testCompleteCount](const TestResult& result) {
        testCompleteCount++;
        std::cout << "  Completed: " << result.name << std::endl;
    });
    
    // Run all tests
    tester->runAllTests();
    
    // Verify callbacks were called
    assert(testStartCount == 4);
    assert(testCompleteCount == 4);
    
    // Check results
    auto results = tester->getTestResults();
    assert(results.size() == 4);
    
    for (const auto& result : results) {
        assert(result.status == TestStatus::COMPLETED);
        assert(result.duration.count() > 0);
    }
    
    std::cout << "✓ Stress Tester Integration passed" << std::endl;
}

void test_system_monitor() {
    std::cout << "Testing System Monitor..." << std::endl;
    
    auto monitor = std::make_shared<SystemMonitor>();
    
    // Set up callback
    int callbackCount = 0;
    monitor->setMetricsCallback([&callbackCount](const SystemMetrics& metrics) {
        callbackCount++;
        assert(metrics.cpuUsage >= 0.0 && metrics.cpuUsage <= 100.0);
        assert(metrics.memoryUsage >= 0.0 && metrics.memoryUsage <= 100.0);
        assert(metrics.temperature >= 0.0 && metrics.temperature <= 100.0);
    });
    
    // Start monitoring
    monitor->startMonitoring();
    assert(monitor->isMonitoring());
    
    // Let it run for a short time
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
    // Stop monitoring
    monitor->stopMonitoring();
    assert(!monitor->isMonitoring());
    
    // Verify callback was called
    assert(callbackCount > 0);
    
    // Check current metrics
    auto metrics = monitor->getCurrentMetrics();
    assert(metrics.cpuUsage >= 0.0);
    assert(metrics.memoryUsage >= 0.0);
    
    std::cout << "✓ System Monitor passed" << std::endl;
}

void test_fault_injector() {
    std::cout << "Testing Fault Injector..." << std::endl;
    
    auto injector = std::make_unique<FaultInjector>();
    
    // Test fault injection
    bool faultInjected = false;
    injector->setFaultInjectedCallback([&faultInjected](const FaultResult& result) {
        faultInjected = true;
        assert(result.success);
        assert(result.type == FaultType::CPU_OVERLOAD);
    });
    
    // Inject a fault
    bool success = injector->injectFault(FaultType::CPU_OVERLOAD, "test_target", FaultSeverity::LOW);
    assert(success);
    assert(faultInjected);
    
    // Check fault history
    auto history = injector->getFaultHistory();
    assert(history.size() == 1);
    assert(history[0].success);
    
    // Test auto-recovery
    std::this_thread::sleep_for(std::chrono::milliseconds(11000)); // Wait for auto-recovery
    
    // Clear faults
    injector->clearFaults();
    
    std::cout << "✓ Fault Injector passed" << std::endl;
}

int main() {
    std::cout << "Running Hardware Stress Tool Unit Tests..." << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try {
        // Initialize logger
        LOGGER.setLogLevel(LogLevel::WARNING); // Reduce log output during tests
        
        test_cpu_stress_test();
        test_memory_stress_test();
        test_stress_tester_integration();
        test_system_monitor();
        test_fault_injector();
        
        std::cout << "\n==========================================" << std::endl;
        std::cout << "🎉 All tests passed successfully!" << std::endl;
        std::cout << "The hardware stress tool is working correctly." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
} 