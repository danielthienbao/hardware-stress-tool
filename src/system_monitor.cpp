#include "system_monitor.h"
#include "logger.h"
#include <algorithm>
#include <random>

namespace hwstress {

SystemMonitor::SystemMonitor() {
    LOGGER.info("SystemMonitor initialized");
}

SystemMonitor::~SystemMonitor() {
    stopMonitoring();
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
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(10.0, 90.0);
    return dis(gen);
}

double SystemMonitor::getMemoryUsage() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(20.0, 80.0);
    return dis(gen);
}

double SystemMonitor::getTemperature() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(30.0, 70.0);
    return dis(gen);
}

double SystemMonitor::getGpuUsage() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 100.0);
    return dis(gen);
}

double SystemMonitor::getGpuTemperature() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(40.0, 85.0);
    return dis(gen);
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