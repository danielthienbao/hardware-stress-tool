#include "fault_injector.h"
#include "logger.h"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#endif

namespace hwstress {

FaultInjector::FaultInjector() {
    LOGGER.info("FaultInjector initialized");
    running_ = true;
    faultThread_ = std::thread(&FaultInjector::faultLoop, this);
}

FaultInjector::~FaultInjector() {
    running_ = false;
    if (faultThread_.joinable()) {
        faultThread_.join();
    }
    
    // Cleanup any active faults
    clearFaults();
}

bool FaultInjector::injectFault(const FaultConfig& config) {
    LOGGER.info("Injecting fault: " + std::to_string(static_cast<int>(config.type)) + 
                " on target: " + config.target);
    
    FaultResult result;
    result.type = config.type;
    result.target = config.target;
    result.severity = config.severity;
    result.injectionTime = std::chrono::system_clock::now();
    result.success = false;
    
    bool success = false;
    
    switch (config.type) {
        case FaultType::MEMORY_CORRUPTION:
            success = injectMemoryCorruption(config.target, config.severity);
            break;
        case FaultType::CPU_OVERLOAD:
            success = injectCpuOverload(config.target, config.severity);
            break;
        case FaultType::DISK_IO_ERROR:
            success = injectDiskIOError(config.target, config.severity);
            break;
        case FaultType::NETWORK_PACKET_LOSS:
            success = injectNetworkPacketLoss(config.target, config.severity);
            break;
        case FaultType::TIMING_ANOMALY:
            success = injectTimingAnomaly(config.target, config.severity);
            break;
        case FaultType::PROCESS_KILL:
            success = injectProcessKill(config.target, config.severity);
            break;
        case FaultType::SYSTEM_CALL_FAILURE:
            success = injectSystemCallFailure(config.target, config.severity);
            break;
        default:
            LOGGER.error("Unknown fault type: " + std::to_string(static_cast<int>(config.type)));
            break;
    }
    
    result.success = success;
    if (success) {
        activeFaults_.push_back(result);
        if (faultInjectedCallback_) {
            faultInjectedCallback_(result);
        }
    }
    
    faultHistory_.push_back(result);
    return success;
}

bool FaultInjector::injectFault(FaultType type, const std::string& target, FaultSeverity severity) {
    FaultConfig config;
    config.type = type;
    config.target = target;
    config.severity = severity;
    config.duration = std::chrono::seconds(10);
    config.probability = 1.0;
    config.autoRecover = true;
    
    return injectFault(config);
}

void FaultInjector::addFault(const FaultConfig& config) {
    pendingFaults_.push_back(config);
    LOGGER.info("Fault added to queue: " + std::to_string(static_cast<int>(config.type)));
}

void FaultInjector::injectAllFaults() {
    LOGGER.info("Injecting all pending faults");
    
    for (const auto& config : pendingFaults_) {
        if (config.probability >= 1.0 || 
            (std::rand() / static_cast<double>(RAND_MAX)) < config.probability) {
            injectFault(config);
        }
    }
    
    pendingFaults_.clear();
}

void FaultInjector::clearFaults() {
    pendingFaults_.clear();
    activeFaults_.clear();
    LOGGER.info("All pending and active faults cleared");
}

void FaultInjector::enableAutoRecovery(bool enable) {
    autoRecoveryEnabled_ = enable;
    LOGGER.info("Auto recovery " + std::string(enable ? "enabled" : "disabled"));
}

void FaultInjector::setRecoveryDelay(std::chrono::milliseconds delay) {
    recoveryDelay_ = delay;
    LOGGER.info("Recovery delay set to: " + std::to_string(delay.count()) + "ms");
}

std::vector<FaultResult> FaultInjector::getFaultHistory() const {
    return faultHistory_;
}

bool FaultInjector::isFaultActive(const std::string& target) const {
    for (const auto& fault : activeFaults_) {
        if (fault.target == target) {
            return true;
        }
    }
    return false;
}

void FaultInjector::setFaultInjectedCallback(std::function<void(const FaultResult&)> callback) {
    faultInjectedCallback_ = callback;
}

void FaultInjector::setFaultRecoveredCallback(std::function<void(const FaultResult&)> callback) {
    faultRecoveredCallback_ = callback;
}

// Platform-specific implementations
bool FaultInjector::injectMemoryCorruption(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting memory corruption on: " + target);
    
    try {
        // Simulate memory corruption by allocating and corrupting memory
        size_t corruptSize = 1024 * 1024; // 1MB
        if (severity == FaultSeverity::HIGH) {
            corruptSize *= 10; // 10MB for high severity
        }
        
        std::vector<uint8_t> corruptBuffer(corruptSize);
        
        // Fill with random data to simulate corruption
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        for (auto& byte : corruptBuffer) {
            byte = dis(gen);
        }
        
        // Keep the buffer alive to simulate memory pressure
        corruptedMemory_.push_back(std::move(corruptBuffer));
        
        // Limit the number of corrupted blocks
        if (corruptedMemory_.size() > 10) {
            corruptedMemory_.erase(corruptedMemory_.begin());
        }
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("Memory corruption injection failed: " + std::string(e.what()));
        return false;
    }
}

bool FaultInjector::injectCpuOverload(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting CPU overload on: " + target);
    
    try {
        int threadCount = 1;
        if (severity == FaultSeverity::MEDIUM) {
            threadCount = 2;
        } else if (severity == FaultSeverity::HIGH) {
            threadCount = 4;
        }
        
        // Create CPU-intensive threads
        for (int i = 0; i < threadCount; ++i) {
            cpuOverloadThreads_.emplace_back([this]() {
                while (running_) {
                    // Perform CPU-intensive calculations
                    volatile double sum = 0.0;
                    for (int j = 0; j < 10000; ++j) {
                        sum += std::sin(j) * std::cos(j);
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            });
        }
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("CPU overload injection failed: " + std::string(e.what()));
        return false;
    }
}

bool FaultInjector::injectDiskIOError(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting disk I/O error on: " + target);
    
    try {
        // Create temporary files and perform I/O operations to simulate disk stress
        std::string tempDir = std::filesystem::temp_directory_path().string() + "/fault_injection";
        std::filesystem::create_directories(tempDir);
        
        int fileCount = 5;
        if (severity == FaultSeverity::HIGH) {
            fileCount = 20;
        }
        
        for (int i = 0; i < fileCount; ++i) {
            std::string filename = tempDir + "/fault_test_" + std::to_string(i) + ".tmp";
            std::ofstream file(filename, std::ios::binary);
            
            if (file.is_open()) {
                // Write random data
                std::vector<char> buffer(1024 * 1024); // 1MB
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 255);
                
                for (auto& c : buffer) {
                    c = static_cast<char>(dis(gen));
                }
                
                file.write(buffer.data(), buffer.size());
                faultTestFiles_.push_back(filename);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("Disk I/O error injection failed: " + std::string(e.what()));
        return false;
    }
}

bool FaultInjector::injectNetworkPacketLoss(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting network packet loss on: " + target);
    
    // Simulate network issues by creating network-intensive operations
    try {
        int connectionCount = 1;
        if (severity == FaultSeverity::HIGH) {
            connectionCount = 5;
        }
        
        // Create dummy network operations (simulated)
        for (int i = 0; i < connectionCount; ++i) {
            networkFaults_.push_back(std::make_unique<std::thread>([this]() {
                while (running_) {
                    // Simulate network operations
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }));
        }
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("Network packet loss injection failed: " + std::string(e.what()));
        return false;
    }
}

bool FaultInjector::injectTimingAnomaly(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting timing anomaly on: " + target);
    
    try {
        // Create timing anomalies by manipulating sleep durations
        timingAnomalyThreads_.emplace_back([this, severity]() {
            while (running_) {
                // Random sleep intervals to simulate timing issues
                std::random_device rd;
                std::mt19937 gen(rd());
                
                int minDelay = 10;
                int maxDelay = 100;
                
                if (severity == FaultSeverity::HIGH) {
                    minDelay = 50;
                    maxDelay = 500;
                }
                
                std::uniform_int_distribution<> dis(minDelay, maxDelay);
                std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
            }
        });
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("Timing anomaly injection failed: " + std::string(e.what()));
        return false;
    }
}

bool FaultInjector::injectProcessKill(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting process kill on: " + target);
    
    try {
#ifdef _WIN32
        // Windows implementation
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            
            if (Process32First(snapshot, &pe32)) {
                do {
                    std::wstring processName = pe32.szExeFile;
                    std::string processNameStr(processName.begin(), processName.end());
                    
                    if (processNameStr.find(target) != std::string::npos) {
                        HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                        if (processHandle != nullptr) {
                            TerminateProcess(processHandle, 1);
                            CloseHandle(processHandle);
                            CloseHandle(snapshot);
                            return true;
                        }
                    }
                } while (Process32Next(snapshot, &pe32));
            }
            CloseHandle(snapshot);
        }
#else
        // Linux implementation
        // Note: This is a simplified implementation
        // In a real scenario, you'd need proper process management
        LOGGER.warning("Process kill injection not fully implemented on Linux");
#endif
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("Process kill injection failed: " + std::string(e.what()));
        return false;
    }
}

bool FaultInjector::injectSystemCallFailure(const std::string& target, FaultSeverity severity) {
    LOGGER.info("Injecting system call failure on: " + target);
    
    try {
        // Simulate system call failures by creating problematic operations
        systemCallFaults_.emplace_back(std::make_unique<std::thread>([this, severity]() {
            while (running_) {
                // Simulate system call failures
                if (severity == FaultSeverity::HIGH) {
                    // More frequent failures for high severity
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }
        }));
        
        return true;
    } catch (const std::exception& e) {
        LOGGER.error("System call failure injection failed: " + std::string(e.what()));
        return false;
    }
}

void FaultInjector::autoRecoverFault(const FaultResult& fault) {
    LOGGER.info("Auto-recovering fault: " + fault.target);
    
    // Remove from active faults
    activeFaults_.erase(
        std::remove_if(activeFaults_.begin(), activeFaults_.end(),
            [&fault](const FaultResult& f) { return f.target == fault.target; }),
        activeFaults_.end()
    );
    
    // Clean up resources based on fault type
    switch (fault.type) {
        case FaultType::MEMORY_CORRUPTION:
            corruptedMemory_.clear();
            break;
        case FaultType::CPU_OVERLOAD:
            for (auto& thread : cpuOverloadThreads_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            cpuOverloadThreads_.clear();
            break;
        case FaultType::DISK_IO_ERROR:
            for (const auto& file : faultTestFiles_) {
                std::filesystem::remove(file);
            }
            faultTestFiles_.clear();
            break;
        case FaultType::NETWORK_PACKET_LOSS:
            for (auto& thread : networkFaults_) {
                if (thread->joinable()) {
                    thread->join();
                }
            }
            networkFaults_.clear();
            break;
        case FaultType::TIMING_ANOMALY:
            for (auto& thread : timingAnomalyThreads_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            timingAnomalyThreads_.clear();
            break;
        case FaultType::SYSTEM_CALL_FAILURE:
            for (auto& thread : systemCallFaults_) {
                if (thread->joinable()) {
                    thread->join();
                }
            }
            systemCallFaults_.clear();
            break;
        default:
            break;
    }
    
    if (faultRecoveredCallback_) {
        faultRecoveredCallback_(fault);
    }
}

void FaultInjector::faultLoop() {
    while (running_) {
        // Check for auto-recovery of faults
        if (autoRecoveryEnabled_) {
            auto now = std::chrono::system_clock::now();
            std::vector<FaultResult> toRecover;
            
            for (const auto& fault : activeFaults_) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                    now - fault.injectionTime);
                
                if (duration >= std::chrono::seconds(10)) { // Auto-recover after 10 seconds
                    toRecover.push_back(fault);
                }
            }
            
            for (const auto& fault : toRecover) {
                autoRecoverFault(fault);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

} // namespace hwstress 