#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <string>
#include <random>
#include <mutex>

enum class FaultType {
    MEMORY_CORRUPTION,
    THREAD_DEADLOCK,
    RESOURCE_EXHAUSTION,
    RANDOM_DELAYS,
    EXCEPTION_INJECTION,
    DISK_IO_FAILURE,
    NETWORK_TIMEOUT
};

struct FaultConfig {
    FaultType type = FaultType::RANDOM_DELAYS;
    double probability = 0.1; // 10% chance
    int durationMs = 100;
    std::string targetComponent = "all";
    bool enableRecovery = true;
};

struct FaultInjectionResult {
    FaultType faultType;
    std::chrono::steady_clock::time_point injectionTime;
    bool recoverySuccessful = false;
    std::string errorMessage;
    int affectedThreads = 0;
};

class FaultInjector {
public:
    FaultInjector();
    ~FaultInjector();

    void enableFaultInjection(const FaultConfig& config);
    void disableFaultInjection();
    void injectFault(FaultType type);
    
    std::vector<FaultInjectionResult> getInjectionHistory();
    void setFaultCallback(std::function<void(const FaultInjectionResult&)> callback);

    // Static methods for specific fault types
    static void injectMemoryCorruption(void* ptr, size_t size);
    static void injectRandomDelay(int minMs = 10, int maxMs = 1000);
    static void injectResourceExhaustion();

private:
    std::atomic<bool> injectionEnabled;
    FaultConfig currentConfig;
    std::vector<FaultInjectionResult> injectionHistory;
    std::function<void(const FaultInjectionResult&)> faultCallback;
    
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
    
    std::mutex historyMutex;
    
    void recordInjection(const FaultInjectionResult& result);
    bool shouldInjectFault();
}; 