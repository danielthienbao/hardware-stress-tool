#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <signal.h>
#include "logger.h"
#include "system_monitor.h"
#include "stress_tester.h"
#include "fault_injector.h"

using namespace hwstress;

// Global variables for cleanup
std::atomic<bool> running{true};
std::shared_ptr<SystemMonitor> systemMonitor;
std::unique_ptr<StressTester> stressTester;
std::unique_ptr<FaultInjector> faultInjector;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down..." << std::endl;
    running = false;
}

void printUsage() {
    std::cout << "Hardware Stress Diagnostic Tool v1.0.0\n"
              << "Usage: hardware-stress-tool [options]\n\n"
              << "Options:\n"
              << "  --help, -h           Show this help message\n"
              << "  --cpu-test           Run CPU stress test\n"
              << "  --memory-test        Run memory stress test\n"
              << "  --gpu-test           Run GPU stress test\n"
              << "  --disk-test          Run disk I/O stress test\n"
              << "  --duration <ms>      Test duration in milliseconds (default: 300000)\n"
              << "  --intensity <1-10>   Test intensity level (default: 5)\n"
              << "  --log-file <file>    Log file path (default: stress_test.log)\n"
              << "  --monitor-only       Only monitor system, don't run stress tests\n"
              << "  --fault-injection    Enable fault injection during tests\n"
              << std::endl;
}

void initializeComponents() {
    // Initialize logger
    LOGGER.setLogFile("stress_test.log");
    LOGGER.setLogLevel(LogLevel::INFO);
    LOGGER.info("Hardware Stress Diagnostic Tool starting...");
    
    // Initialize system monitor
    systemMonitor = std::make_shared<SystemMonitor>();
    systemMonitor->setMetricsCallback([](const SystemMetrics& metrics) {
        LOGGER.logSystemMetrics(metrics.cpuUsage, metrics.memoryUsage, metrics.temperature);
    });
    
    // Initialize stress tester
    stressTester = std::make_unique<StressTester>();
    stressTester->setSystemMonitor(systemMonitor);
    
    // Add concrete stress tests
    stressTester->addTest(std::make_unique<CpuStressTest>());
    stressTester->addTest(std::make_unique<MemoryStressTest>());
    stressTester->addTest(std::make_unique<DiskStressTest>());
    stressTester->addTest(std::make_unique<GpuStressTest>());
    
    // Initialize fault injector
    faultInjector = std::make_unique<FaultInjector>();
    
    LOGGER.info("All components initialized successfully");
}

void runSystemMonitoring() {
    LOGGER.info("Starting system monitoring...");
    systemMonitor->startMonitoring();
    
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    systemMonitor->stopMonitoring();
    LOGGER.info("System monitoring stopped");
}

void runStressTests(bool runCpuTest, bool runMemoryTest, bool runGpuTest, bool runDiskTest, 
                   std::chrono::milliseconds duration, int intensity) {
    LOGGER.info("Starting stress tests...");
    LOGGER.info("Duration: " + std::to_string(duration.count()) + "ms");
    LOGGER.info("Intensity: " + std::to_string(intensity));
    
    // Set global configuration
    stressTester->setGlobalDuration(duration);
    stressTester->setGlobalIntensity(intensity);
    
    // Set up callbacks for test progress
    stressTester->setTestStartCallback([](const std::string& testName) {
        LOGGER.info("Starting test: " + testName);
    });
    
    stressTester->setTestCompleteCallback([](const TestResult& result) {
        LOGGER.info("Test completed: " + result.name + " (Status: " + 
                   std::to_string(static_cast<int>(result.status)) + ")");
    });
    
    // Start system monitoring
    systemMonitor->startMonitoring();
    
    // Run specific tests based on command line arguments
    if (runCpuTest) {
        stressTester->runTest("CPU Stress Test");
    }
    
    if (runMemoryTest) {
        stressTester->runTest("Memory Stress Test");
    }
    
    if (runGpuTest) {
        stressTester->runTest("GPU Stress Test");
    }
    
    if (runDiskTest) {
        stressTester->runTest("Disk Stress Test");
    }
    
    // If no specific tests were requested, run all tests
    if (!runCpuTest && !runMemoryTest && !runGpuTest && !runDiskTest) {
        stressTester->runAllTests();
    }
    
    // Stop system monitoring
    systemMonitor->stopMonitoring();
    
    // Print test results
    auto results = stressTester->getTestResults();
    LOGGER.info("Test Results Summary:");
    for (const auto& result : results) {
        LOGGER.info("  " + result.name + ": " + 
                   std::to_string(result.duration.count()) + "ms");
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    bool runCpuTest = false;
    bool runMemoryTest = false;
    bool runGpuTest = false;
    bool runDiskTest = false;
    bool monitorOnly = false;
    bool enableFaultInjection = false;
    std::chrono::milliseconds duration{300000}; // 5 minutes default
    int intensity = 5;
    std::string logFile = "stress_test.log";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--cpu-test") {
            runCpuTest = true;
        } else if (arg == "--memory-test") {
            runMemoryTest = true;
        } else if (arg == "--gpu-test") {
            runGpuTest = true;
        } else if (arg == "--disk-test") {
            runDiskTest = true;
        } else if (arg == "--duration" && i + 1 < argc) {
            duration = std::chrono::milliseconds(std::stoi(argv[++i]));
        } else if (arg == "--intensity" && i + 1 < argc) {
            intensity = std::stoi(argv[++i]);
            if (intensity < 1 || intensity > 10) {
                std::cerr << "Error: Intensity must be between 1 and 10" << std::endl;
                return 1;
            }
        } else if (arg == "--log-file" && i + 1 < argc) {
            logFile = argv[++i];
        } else if (arg == "--monitor-only") {
            monitorOnly = true;
        } else if (arg == "--fault-injection") {
            enableFaultInjection = true;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }
    
    try {
        // Initialize components
        initializeComponents();
        LOGGER.setLogFile(logFile);
        
        if (monitorOnly) {
            // Run only system monitoring
            runSystemMonitoring();
        } else {
            // Run stress tests
            runStressTests(runCpuTest, runMemoryTest, runGpuTest, runDiskTest, duration, intensity);
        }
        
    } catch (const std::exception& e) {
        LOGGER.error("Fatal error: " + std::string(e.what()));
        return 1;
    }
    
    LOGGER.info("Hardware Stress Diagnostic Tool finished");
    return 0;
} 