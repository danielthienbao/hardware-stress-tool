#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <mutex>

namespace hwstress {

struct SystemMetrics {
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double temperature = 0.0;
    double gpuUsage = 0.0;
    double gpuTemperature = 0.0;
    uint64_t totalMemory = 0;
    uint64_t availableMemory = 0;
    std::chrono::system_clock::time_point timestamp;
};

struct ProcessInfo {
    int pid;
    std::string name;
    double cpuUsage;
    uint64_t memoryUsage;
    std::string status;
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    SystemMetrics getCurrentMetrics() const;
    std::vector<ProcessInfo> getProcessList() const;
    
    void setMonitoringInterval(int intervalMs);
    void setMetricsCallback(std::function<void(const SystemMetrics&)> callback);
    
    // Platform-specific implementations
    double getCpuUsage();
    double getMemoryUsage();
    double getTemperature();
    double getGpuUsage();
    double getGpuTemperature();
    
    std::vector<ProcessInfo> getTopProcesses(int count = 10);
    ProcessInfo getProcessInfo(int pid);

private:
    void monitoringLoop();
    void updateMetrics();
    
    std::thread monitoringThread_;
    std::atomic<bool> running_{false};
    std::atomic<int> monitoringInterval_{1000};
    
    mutable std::mutex metricsMutex_;
    SystemMetrics currentMetrics_;
    
    std::function<void(const SystemMetrics&)> metricsCallback_;
    
    // Platform-specific data
    #ifdef _WIN32
    // Windows-specific members
    #else
    // Linux-specific members
    #endif
};

} // namespace hwstress 