#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

struct SystemMetrics {
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    size_t totalMemory = 0;
    size_t availableMemory = 0;
    double diskUsage = 0.0;
    size_t totalDisk = 0;
    size_t availableDisk = 0;
    std::vector<double> coreUsage;
    double temperature = 0.0; // CPU temperature if available
};

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();

    void startMonitoring(int intervalMs = 1000);
    void stopMonitoring();
    SystemMetrics getCurrentMetrics();
    void setMonitoringCallback(std::function<void(const SystemMetrics&)> callback);

    // Static utility functions
    static int getCoreCount();
    static std::string getSystemInfo();

private:
    std::atomic<bool> monitoring;
    std::unique_ptr<std::thread> monitorThread;
    SystemMetrics currentMetrics;
    std::mutex metricsMutex;
    std::function<void(const SystemMetrics&)> metricsCallback;

    void monitoringLoop(int intervalMs);
    void updateCpuUsage();
    void updateMemoryUsage();
    void updateDiskUsage();
    void updateTemperature();

#ifdef _WIN32
    void* pdh_cpu_query = nullptr;
    void* pdh_cpu_counter = nullptr;
    void* pdh_memory_counter = nullptr;
#else
    std::vector<size_t> lastCpuTimes;
    std::vector<size_t> lastIdleTimes;
#endif
}; 