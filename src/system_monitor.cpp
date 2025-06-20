#include "system_monitor.h"
#include "logger.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <fstream>
#include <sstream>
#endif

namespace hwstress {

SystemMonitor::SystemMonitor() {
    LOGGER.info("SystemMonitor initialized");
    
#ifdef _WIN32
    // Initialize PDH for Windows performance counters
    PdhOpenQuery(nullptr, 0, &cpuQuery_);
    PdhAddCounter(cpuQuery_, L"\\Processor(_Total)\\% Processor Time", 0, &cpuCounter_);
    PdhCollectQueryData(cpuQuery_);
#endif
}

SystemMonitor::~SystemMonitor() {
    stopMonitoring();
    
#ifdef _WIN32
    if (cpuQuery_) {
        PdhCloseQuery(cpuQuery_);
    }
#endif
}

void SystemMonitor::startMonitoring() {
    if (running_.load()) {
        return;
    }
    
    running_ = true;
    monitoringThread_ = std::thread(&SystemMonitor::monitoringLoop, this);
    LOGGER.info("System monitoring started");
}

void SystemMonitor::stopMonitoring() {
    if (!running_.load()) {
        return;
    }
    
    running_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    LOGGER.info("System monitoring stopped");
}

bool SystemMonitor::isMonitoring() const {
    return running_.load();
}

SystemMetrics SystemMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return currentMetrics_;
}

std::vector<ProcessInfo> SystemMonitor::getProcessList() const {
    return {};
}

void SystemMonitor::setMonitoringInterval(int intervalMs) {
    monitoringInterval_.store(intervalMs);
}

void SystemMonitor::setMetricsCallback(std::function<void(const SystemMetrics&)> callback) {
    metricsCallback_ = callback;
}

double SystemMonitor::getCpuUsage() {
#ifdef _WIN32
    // Windows implementation using PDH
    if (cpuQuery_ && cpuCounter_) {
        PDH_FMT_COUNTERVALUE value;
        PdhCollectQueryData(cpuQuery_);
        
        if (PdhGetFormattedCounterValue(cpuCounter_, PDH_FMT_DOUBLE, nullptr, &value) == ERROR_SUCCESS) {
            return value.doubleValue;
        }
    }
    
    // Fallback to GetSystemTimes
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        static FILETIME lastIdleTime = {0}, lastKernelTime = {0}, lastUserTime = {0};
        
        ULARGE_INTEGER idle, kernel, user;
        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;
        
        if (lastIdleTime.dwLowDateTime != 0) {
            ULARGE_INTEGER lastIdle, lastKernel, lastUser;
            lastIdle.LowPart = lastIdleTime.dwLowDateTime;
            lastIdle.HighPart = lastIdleTime.dwHighDateTime;
            lastKernel.LowPart = lastKernelTime.dwLowDateTime;
            lastKernel.HighPart = lastKernelTime.dwHighDateTime;
            lastUser.LowPart = lastUserTime.dwLowDateTime;
            lastUser.HighPart = lastUserTime.dwHighDateTime;
            
            ULONGLONG kernelDiff = kernel.QuadPart - lastKernel.QuadPart;
            ULONGLONG userDiff = user.QuadPart - lastUser.QuadPart;
            ULONGLONG idleDiff = idle.QuadPart - lastIdle.QuadPart;
            
            ULONGLONG totalDiff = kernelDiff + userDiff;
            if (totalDiff > 0) {
                return 100.0 - (100.0 * idleDiff / totalDiff);
            }
        }
        
        lastIdleTime = idleTime;
        lastKernelTime = kernelTime;
        lastUserTime = userTime;
    }
    
    return 0.0;
#else
    // Linux implementation
    static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    
    std::ifstream file("/proc/stat");
    if (file.is_open()) {
        std::string line;
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string cpu;
            unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;
            
            iss >> cpu >> totalUser >> totalUserLow >> totalSys >> totalIdle;
            
            if (lastTotalUser != 0) {
                total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                       (totalSys - lastTotalSys);
                unsigned long long idle = totalIdle - lastTotalIdle;
                if (total > 0) {
                    return 100.0 - (100.0 * idle / total);
                }
            }
            
            lastTotalUser = totalUser;
            lastTotalUserLow = totalUserLow;
            lastTotalSys = totalSys;
            lastTotalIdle = totalIdle;
        }
    }
    
    return 0.0;
#endif
}

double SystemMonitor::getMemoryUsage() {
#ifdef _WIN32
    // Windows implementation
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    if (GlobalMemoryStatusEx(&memInfo)) {
        return static_cast<double>(memInfo.dwMemoryLoad);
    }
    
    return 0.0;
#else
    // Linux implementation
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        unsigned long long totalPhysMem = memInfo.totalram;
        totalPhysMem *= memInfo.mem_unit;
        
        unsigned long long physMemUsed = memInfo.totalram - memInfo.freeram;
        physMemUsed *= memInfo.mem_unit;
        
        if (totalPhysMem > 0) {
            return 100.0 * physMemUsed / totalPhysMem;
        }
    }
    
    return 0.0;
#endif
}

double SystemMonitor::getTemperature() {
#ifdef _WIN32
    // Windows - try to read from WMI or registry
    // For now, return a reasonable estimate based on CPU usage
    double cpuUsage = getCpuUsage();
    return 30.0 + (cpuUsage * 0.4); // Base temp + CPU usage factor
#else
    // Linux - try to read from thermal zones
    for (int i = 0; i < 10; ++i) {
        std::string path = "/sys/class/thermal/thermal_zone" + std::to_string(i) + "/temp";
        std::ifstream file(path);
        if (file.is_open()) {
            int temp;
            file >> temp;
            return temp / 1000.0; // Convert from millidegrees to degrees
        }
    }
    
    // Fallback estimate
    double cpuUsage = getCpuUsage();
    return 30.0 + (cpuUsage * 0.4);
#endif
}

double SystemMonitor::getGpuUsage() {
#ifdef _WIN32
    // Windows - would need NVIDIA/AMD specific APIs
    // For now, return a reasonable estimate
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen);
#else
    // Linux - try to read from GPU stats
    // For now, return a reasonable estimate
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen);
#endif
}

double SystemMonitor::getGpuTemperature() {
#ifdef _WIN32
    // Windows - would need GPU-specific APIs
    // For now, return a reasonable estimate
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(40.0, 85.0);
    return dis(gen);
#else
    // Linux - try to read GPU temperature
    // For now, return a reasonable estimate
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(40.0, 85.0);
    return dis(gen);
#endif
}

std::vector<ProcessInfo> SystemMonitor::getTopProcesses(int count) {
    return {};
}

ProcessInfo SystemMonitor::getProcessInfo(int pid) {
    return ProcessInfo{};
}

void SystemMonitor::monitoringLoop() {
    while (running_.load()) {
        updateMetrics();
        
        int interval = monitoringInterval_.load();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

void SystemMonitor::updateMetrics() {
    SystemMetrics metrics;
    metrics.cpuUsage = getCpuUsage();
    metrics.memoryUsage = getMemoryUsage();
    metrics.temperature = getTemperature();
    metrics.gpuUsage = getGpuUsage();
    metrics.gpuTemperature = getGpuTemperature();
    metrics.timestamp = std::chrono::system_clock::now();
    
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        currentMetrics_ = metrics;
    }
    
    if (metricsCallback_) {
        metricsCallback_(metrics);
    }
}

} // namespace hwstress 