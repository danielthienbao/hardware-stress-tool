#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>

namespace hwstress {

enum class FaultType {
    MEMORY_CORRUPTION,
    CPU_OVERLOAD,
    DISK_IO_ERROR,
    NETWORK_PACKET_LOSS,
    TIMING_ANOMALY,
    PROCESS_KILL,
    SYSTEM_CALL_FAILURE,
    CUSTOM_FAULT
};

enum class FaultSeverity {
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

struct FaultConfig {
    FaultType type;
    FaultSeverity severity;
    std::string target;
    std::chrono::milliseconds duration;
    double probability;
    std::vector<std::string> parameters;
    bool autoRecover;
};

struct FaultResult {
    FaultType type;
    std::string target;
    FaultSeverity severity;
    bool success;
    std::string errorMessage;
    std::chrono::system_clock::time_point injectionTime;
    std::chrono::system_clock::time_point timestamp;
    std::chrono::milliseconds duration;
};

class FaultInjector {
public:
    FaultInjector();
    ~FaultInjector();
    
    bool injectFault(const FaultConfig& config);
    bool injectFault(FaultType type, const std::string& target, 
                    FaultSeverity severity = FaultSeverity::MEDIUM);
    
    void addFault(const FaultConfig& config);
    void injectAllFaults();
    void clearFaults();
    
    void enableAutoRecovery(bool enable);
    void setRecoveryDelay(std::chrono::milliseconds delay);
    
    std::vector<FaultResult> getFaultHistory() const;
    bool isFaultActive(const std::string& target) const;
    
    void setFaultInjectedCallback(std::function<void(const FaultResult&)> callback);
    void setFaultRecoveredCallback(std::function<void(const FaultResult&)> callback);

private:
    bool injectMemoryCorruption(const std::string& target, FaultSeverity severity);
    bool injectCpuOverload(const std::string& target, FaultSeverity severity);
    bool injectDiskIOError(const std::string& target, FaultSeverity severity);
    bool injectNetworkPacketLoss(const std::string& target, FaultSeverity severity);
    bool injectTimingAnomaly(const std::string& target, FaultSeverity severity);
    bool injectProcessKill(const std::string& target, FaultSeverity severity);
    bool injectSystemCallFailure(const std::string& target, FaultSeverity severity);
    
    void autoRecoverFault(const FaultResult& fault);
    void faultLoop();
    
    std::vector<FaultConfig> pendingFaults_;
    std::vector<FaultResult> faultHistory_;
    std::vector<FaultResult> activeFaults_;
    
    bool autoRecoveryEnabled_{true};
    std::chrono::milliseconds recoveryDelay_{std::chrono::seconds(5)};
    
    std::function<void(const FaultResult&)> faultInjectedCallback_;
    std::function<void(const FaultResult&)> faultRecoveredCallback_;
    
    // Thread management
    std::thread faultThread_;
    std::atomic<bool> running_{false};
    
    // Fault-specific resources
    std::vector<std::vector<uint8_t>> corruptedMemory_;
    std::vector<std::thread> cpuOverloadThreads_;
    std::vector<std::string> faultTestFiles_;
    std::vector<std::unique_ptr<std::thread>> networkFaults_;
    std::vector<std::thread> timingAnomalyThreads_;
    std::vector<std::unique_ptr<std::thread>> systemCallFaults_;
};

} // namespace hwstress 