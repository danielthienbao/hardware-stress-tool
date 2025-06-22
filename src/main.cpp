#include "stress_tester.h"
#include "system_monitor.h"
#include "fault_injector.h"
#include "logger.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

void printBanner() {
    std::cout << "========================================\n";
    std::cout << "    Hardware Stress Diagnostic Tool    \n";
    std::cout << "========================================\n";
    std::cout << "Cross-platform hardware stress testing\n";
    std::cout << "and system monitoring utility\n\n";
}

void printSystemInfo() {
    std::cout << "System Information:\n";
    std::cout << "==================\n";
    std::cout << SystemMonitor::getSystemInfo() << std::endl;
}

void printMenu() {
    std::cout << "\nSelect test type:\n";
    std::cout << "1. CPU Stress Test\n";
    std::cout << "2. Memory Stress Test\n";
    std::cout << "3. Disk Stress Test\n";
    std::cout << "4. GPU Simulation Test\n";
    std::cout << "5. Combined Stress Test\n";
    std::cout << "6. System Monitor Demo\n";
    std::cout << "7. Fault Injection Demo\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
}

void runStressTest(StressType type, const std::string& testName) {
    std::cout << "\n" << testName << " Starting...\n";
    std::cout << "Duration: 10 seconds\n";
    std::cout << "Progress: [";
    
    StressTester tester;
    StressTestConfig config;
    config.type = type;
    config.durationSeconds = 10;
    config.threadCount = 0; // Auto-detect
    
    // Set up progress callback
    int lastProgress = 0;
    tester.setProgressCallback([&](double progress) {
        int currentProgress = static_cast<int>(progress * 20);
        while (lastProgress < currentProgress) {
            std::cout << "=";
            std::cout.flush();
            lastProgress++;
        }
    });
    
    // Set up results callback
    tester.setResultCallback([&](const StressTestResults& results) {
        std::cout << "]\n\n";
        std::cout << "Test Results:\n";
        std::cout << "=============\n";
        std::cout << "Duration: " << results.actualDuration.count() << " seconds\n";
        std::cout << "Operations: " << results.operationsCompleted << "\n";
        std::cout << "Errors: " << results.errorsEncountered << "\n";
        std::cout << "Status: " << (results.testPassed ? "PASSED" : "FAILED") << "\n";
        if (!results.additionalInfo.empty()) {
            std::cout << "Info: " << results.additionalInfo << "\n";
        }
    });
    
    tester.startTest(config);
    
    // Wait for test completion
    while (tester.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "\nPress any key to continue...";
    std::cin.get();
}

void runSystemMonitorDemo() {
    std::cout << "\nSystem Monitor Demo\n";
    std::cout << "===================\n";
    std::cout << "Monitoring system metrics for 15 seconds...\n\n";
    
    SystemMonitor monitor;
    
    // Set up monitoring callback
    monitor.setMonitoringCallback([](const SystemMetrics& metrics) {
        std::cout << "\r";
        std::cout << "CPU: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%  ";
        std::cout << "Memory: " << std::fixed << std::setprecision(1) << metrics.memoryUsage << "%  ";
        std::cout << "Disk: " << std::fixed << std::setprecision(1) << metrics.diskUsage << "%  ";
        std::cout << "RAM: " << (metrics.totalMemory / (1024*1024)) << "MB total  ";
        std::cout.flush();
    });
    
    monitor.startMonitoring(1000); // Update every second
    
    // Run for 15 seconds
    std::this_thread::sleep_for(std::chrono::seconds(15));
    
    monitor.stopMonitoring();
    
    std::cout << "\n\nMonitoring completed.\n";
    std::cout << "Press any key to continue...";
    std::cin.get();
}

void runFaultInjectionDemo() {
    std::cout << "\nFault Injection Demo\n";
    std::cout << "====================\n";
    std::cout << "Demonstrating fault injection capabilities...\n\n";
    
    FaultInjector injector;
    
    // Configure fault injection
    FaultConfig config;
    config.probability = 1.0; // 100% for demo
    config.enableRecovery = true;
    config.durationMs = 200;
    
    injector.enableFaultInjection(config);
    
    // Set up fault callback
    injector.setFaultCallback([](const FaultInjectionResult& result) {
        std::cout << "Fault injected: ";
        switch (result.faultType) {
            case FaultType::RANDOM_DELAYS: std::cout << "Random Delay"; break;
            case FaultType::RESOURCE_EXHAUSTION: std::cout << "Resource Exhaustion"; break;
            case FaultType::EXCEPTION_INJECTION: std::cout << "Exception Injection"; break;
            case FaultType::MEMORY_CORRUPTION: std::cout << "Memory Corruption"; break;
            case FaultType::THREAD_DEADLOCK: std::cout << "Thread Deadlock"; break;
            case FaultType::DISK_IO_FAILURE: std::cout << "Disk I/O Failure"; break;
            case FaultType::NETWORK_TIMEOUT: std::cout << "Network Timeout"; break;
        }
        std::cout << " - Recovery: " << (result.recoverySuccessful ? "SUCCESS" : "FAILED");
        if (!result.errorMessage.empty()) {
            std::cout << " (" << result.errorMessage << ")";
        }
        std::cout << std::endl;
    });
    
    // Inject different types of faults
    std::vector<FaultType> faultTypes = {
        FaultType::RANDOM_DELAYS,
        FaultType::RESOURCE_EXHAUSTION,
        FaultType::EXCEPTION_INJECTION,
        FaultType::MEMORY_CORRUPTION,
        FaultType::THREAD_DEADLOCK,
        FaultType::DISK_IO_FAILURE,
        FaultType::NETWORK_TIMEOUT
    };
    
    for (auto faultType : faultTypes) {
        injector.injectFault(faultType);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    injector.disableFaultInjection();
    
    std::cout << "\nFault injection demo completed.\n";
    std::cout << "Press any key to continue...";
    std::cin.get();
}

int main() {
    // Initialize logger
    Logger::getInstance().initialize("stress_tool.log");
    Logger::getInstance().enableConsoleOutput(false); // Disable console output for cleaner UI
    
    printBanner();
    printSystemInfo();
    
    int choice;
    do {
        printMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear the input buffer
        
        switch (choice) {
            case 1:
                runStressTest(StressType::CPU, "CPU Stress Test");
                break;
            case 2:
                runStressTest(StressType::MEMORY, "Memory Stress Test");
                break;
            case 3:
                runStressTest(StressType::DISK, "Disk Stress Test");
                break;
            case 4:
                runStressTest(StressType::GPU_SIMULATION, "GPU Simulation Test");
                break;
            case 5:
                runStressTest(StressType::COMBINED, "Combined Stress Test");
                break;
            case 6:
                runSystemMonitorDemo();
                break;
            case 7:
                runFaultInjectionDemo();
                break;
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 0);
    
    Logger::getInstance().log(LogLevel::INFO, "Application terminated normally");
    return 0;
} 