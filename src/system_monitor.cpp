#include "system_monitor.h"
#include "logger.h"
#include <iostream>
#include <algorithm>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#include <psapi.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "psapi.lib")
#else
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#endif

SystemMonitor::SystemMonitor() : monitoring(false) {
#ifdef _WIN32
    PdhOpenQuery(NULL, 0, (PDH_HQUERY*)&pdh_cpu_query);
    PdhAddEnglishCounter((PDH_HQUERY)pdh_cpu_query, L"\\Processor(_Total)\\% Processor Time", 0, (PDH_HCOUNTER*)&pdh_cpu_counter);
    PdhCollectQueryData((PDH_HQUERY)pdh_cpu_query);
#else
    lastCpuTimes.resize(getCoreCount());
    lastIdleTimes.resize(getCoreCount());
#endif
}

SystemMonitor::~SystemMonitor() {
    stopMonitoring();
#ifdef _WIN32
    if (pdh_cpu_query) {
        PdhCloseQuery((PDH_HQUERY)pdh_cpu_query);
    }
#endif
}

void SystemMonitor::startMonitoring(int intervalMs) {
    if (monitoring) {
        return;
    }
    
    monitoring = true;
    monitorThread = std::make_unique<std::thread>(&SystemMonitor::monitoringLoop, this, intervalMs);
    Logger::getInstance().log(LogLevel::INFO, "System monitoring started");
}

void SystemMonitor::stopMonitoring() {
    if (!monitoring) {
        return;
    }
    
    monitoring = false;
    if (monitorThread && monitorThread->joinable()) {
        monitorThread->join();
    }
    Logger::getInstance().log(LogLevel::INFO, "System monitoring stopped");
}

SystemMetrics SystemMonitor::getCurrentMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    return currentMetrics;
}

void SystemMonitor::setMonitoringCallback(std::function<void(const SystemMetrics&)> callback) {
    metricsCallback = callback;
}

int SystemMonitor::getCoreCount() {
    return std::thread::hardware_concurrency();
}

std::string SystemMonitor::getSystemInfo() {
    std::stringstream ss;
    ss << "CPU Cores: " << getCoreCount() << "\n";
    
#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    ss << "Page size: " << sysInfo.dwPageSize << " bytes\n";
    
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    ss << "Total RAM: " << (memInfo.ullTotalPhys / (1024 * 1024)) << " MB\n";
#else
    struct sysinfo info;
    sysinfo(&info);
    ss << "Total RAM: " << (info.totalram * info.mem_unit / (1024 * 1024)) << " MB\n";
    ss << "Uptime: " << info.uptime << " seconds\n";
#endif
    
    return ss.str();
}

void SystemMonitor::monitoringLoop(int intervalMs) {
    while (monitoring) {
        updateCpuUsage();
        updateMemoryUsage();
        updateDiskUsage();
        updateTemperature();
        
        {
            std::lock_guard<std::mutex> lock(metricsMutex);
            if (metricsCallback) {
                metricsCallback(currentMetrics);
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}

void SystemMonitor::updateCpuUsage() {
#ifdef _WIN32
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData((PDH_HQUERY)pdh_cpu_query);
    PdhGetFormattedCounterValue((PDH_HCOUNTER)pdh_cpu_counter, PDH_FMT_DOUBLE, NULL, &counterVal);
    
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics.cpuUsage = counterVal.doubleValue;
#else
    std::ifstream statFile("/proc/stat");
    std::string line;
    std::getline(statFile, line);
    
    std::istringstream iss(line);
    std::string cpu;
    iss >> cpu;
    
    std::vector<size_t> times;
    size_t time;
    while (iss >> time) {
        times.push_back(time);
    }
    
    if (times.size() >= 4) {
        size_t idleTime = times[3];
        size_t totalTime = 0;
        for (size_t t : times) {
            totalTime += t;
        }
        
        static size_t lastTotalTime = 0;
        static size_t lastIdleTime = 0;
        
        if (lastTotalTime > 0) {
            size_t totalDiff = totalTime - lastTotalTime;
            size_t idleDiff = idleTime - lastIdleTime;
            
            std::lock_guard<std::mutex> lock(metricsMutex);
            currentMetrics.cpuUsage = totalDiff > 0 ? (100.0 * (totalDiff - idleDiff) / totalDiff) : 0.0;
        }
        
        lastTotalTime = totalTime;
        lastIdleTime = idleTime;
    }
#endif
}

void SystemMonitor::updateMemoryUsage() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics.totalMemory = memInfo.ullTotalPhys;
    currentMetrics.availableMemory = memInfo.ullAvailPhys;
    currentMetrics.memoryUsage = 100.0 * (1.0 - double(memInfo.ullAvailPhys) / double(memInfo.ullTotalPhys));
#else
    struct sysinfo info;
    sysinfo(&info);
    
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics.totalMemory = info.totalram * info.mem_unit;
    currentMetrics.availableMemory = info.freeram * info.mem_unit;
    currentMetrics.memoryUsage = 100.0 * (1.0 - double(info.freeram) / double(info.totalram));
#endif
}

void SystemMonitor::updateDiskUsage() {
#ifdef _WIN32
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes;
    if (GetDiskFreeSpaceEx(L"C:\\", &freeBytesAvailable, &totalNumberOfBytes, NULL)) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        currentMetrics.totalDisk = totalNumberOfBytes.QuadPart;
        currentMetrics.availableDisk = freeBytesAvailable.QuadPart;
        currentMetrics.diskUsage = 100.0 * (1.0 - double(freeBytesAvailable.QuadPart) / double(totalNumberOfBytes.QuadPart));
    }
#else
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        currentMetrics.totalDisk = stat.f_blocks * stat.f_frsize;
        currentMetrics.availableDisk = stat.f_bavail * stat.f_frsize;
        currentMetrics.diskUsage = 100.0 * (1.0 - double(stat.f_bavail) / double(stat.f_blocks));
    }
#endif
}

void SystemMonitor::updateTemperature() {
    // Temperature monitoring is platform-specific and requires additional libraries
    // For now, we'll set a placeholder value
    std::lock_guard<std::mutex> lock(metricsMutex);
    currentMetrics.temperature = 0.0; // Not implemented
} 