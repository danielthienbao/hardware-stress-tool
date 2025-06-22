#include "fault_injector.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

FaultInjector::FaultInjector() : injectionEnabled(false), gen(rd()) {}

FaultInjector::~FaultInjector() {
    disableFaultInjection();
}

void FaultInjector::enableFaultInjection(const FaultConfig& config) {
    currentConfig = config;
    injectionEnabled = true;
    Logger::getInstance().log(LogLevel::INFO, "Fault injection enabled");
}

void FaultInjector::disableFaultInjection() {
    injectionEnabled = false;
    Logger::getInstance().log(LogLevel::INFO, "Fault injection disabled");
}

void FaultInjector::injectFault(FaultType type) {
    if (!injectionEnabled || !shouldInjectFault()) {
        return;
    }

    FaultInjectionResult result;
    result.faultType = type;
    result.injectionTime = std::chrono::steady_clock::now();

    try {
        switch (type) {
            case FaultType::RANDOM_DELAYS:
                injectRandomDelay(50, currentConfig.durationMs);
                result.recoverySuccessful = true;
                break;
                
            case FaultType::RESOURCE_EXHAUSTION:
                injectResourceExhaustion();
                result.recoverySuccessful = true;
                break;
                
            case FaultType::EXCEPTION_INJECTION:
                if (currentConfig.enableRecovery) {
                    // Simulate recoverable exception
                    result.errorMessage = "Simulated recoverable exception";
                    result.recoverySuccessful = true;
                } else {
                    throw std::runtime_error("Injected fault exception");
                }
                break;
                
            case FaultType::MEMORY_CORRUPTION:
                // For safety, we'll only log this type of fault
                result.errorMessage = "Memory corruption simulation (logged only for safety)";
                result.recoverySuccessful = true;
                break;
                
            case FaultType::THREAD_DEADLOCK:
                // Simulate deadlock detection and recovery
                result.errorMessage = "Deadlock simulation detected and avoided";
                result.recoverySuccessful = true;
                break;
                
            case FaultType::DISK_IO_FAILURE:
                // Simulate I/O failure
                result.errorMessage = "Disk I/O failure simulated";
                result.recoverySuccessful = currentConfig.enableRecovery;
                break;
                
            case FaultType::NETWORK_TIMEOUT:
                // Simulate network timeout
                result.errorMessage = "Network timeout simulated";
                result.recoverySuccessful = currentConfig.enableRecovery;
                break;
        }
        
        Logger::getInstance().log(LogLevel::WARNING, 
            "Fault injected: " + result.errorMessage);
            
    } catch (const std::exception& e) {
        result.errorMessage = e.what();
        result.recoverySuccessful = false;
        Logger::getInstance().log(LogLevel::ERROR, 
            "Fault injection caused unrecoverable error: " + result.errorMessage);
    }

    recordInjection(result);
}

std::vector<FaultInjectionResult> FaultInjector::getInjectionHistory() {
    std::lock_guard<std::mutex> lock(historyMutex);
    return injectionHistory;
}

void FaultInjector::setFaultCallback(std::function<void(const FaultInjectionResult&)> callback) {
    faultCallback = callback;
}

void FaultInjector::injectMemoryCorruption(void* ptr, size_t size) {
    // For safety reasons, we won't actually corrupt memory in a real implementation
    // This would be used in a controlled testing environment
    Logger::getInstance().log(LogLevel::WARNING, 
        "Memory corruption injection requested for " + std::to_string(size) + " bytes (simulation only)");
}

void FaultInjector::injectRandomDelay(int minMs, int maxMs) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minMs, maxMs);
    
    int delay = dis(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    
    Logger::getInstance().log(LogLevel::DEBUG, 
        "Random delay injected: " + std::to_string(delay) + "ms");
}

void FaultInjector::injectResourceExhaustion() {
    // Simulate temporary resource exhaustion
    Logger::getInstance().log(LogLevel::WARNING, "Resource exhaustion simulated");
    
    // Brief delay to simulate resource recovery
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void FaultInjector::recordInjection(const FaultInjectionResult& result) {
    std::lock_guard<std::mutex> lock(historyMutex);
    injectionHistory.push_back(result);
    
    // Limit history size to prevent memory issues
    if (injectionHistory.size() > 1000) {
        injectionHistory.erase(injectionHistory.begin());
    }
    
    if (faultCallback) {
        faultCallback(result);
    }
}

bool FaultInjector::shouldInjectFault() {
    return dis(gen) < currentConfig.probability;
} 