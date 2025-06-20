#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <memory>
#include "system_monitor.h"

namespace hwstress {

enum class TestType {
    CPU_STRESS,
    MEMORY_STRESS,
    GPU_STRESS,
    DISK_STRESS,
    NETWORK_STRESS,
    TEMPERATURE_STRESS,
    CUSTOM_STRESS
};

enum class TestStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED,
    TIMEOUT,
    INTERRUPTED
};

struct TestResult {
    TestType type;
    TestStatus status;
    std::string name;
    std::chrono::milliseconds duration;
    std::string errorMessage;
    SystemMetrics baselineMetrics;
    SystemMetrics peakMetrics;
    std::vector<SystemMetrics> metricsHistory;
};

struct TestConfig {
    TestType type;
    std::string name;
    std::chrono::milliseconds duration;
    int intensity;
    bool monitorMetrics;
    std::vector<std::string> customParams;
};

class StressTest {
public:
    virtual ~StressTest() = default;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual TestResult getResult() const = 0;
    virtual void setConfig(const TestConfig& config) = 0;
    
protected:
    TestConfig config_;
    TestResult result_;
    std::atomic<bool> running_{false};
    std::chrono::system_clock::time_point startTime_;
};

// Concrete stress test implementations
class CpuStressTest : public StressTest {
public:
    CpuStressTest();
    ~CpuStressTest();
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    TestResult getResult() const override;
    void setConfig(const TestConfig& config) override;

private:
    void cpuStressLoop();
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> stopRequested_{false};
};

class MemoryStressTest : public StressTest {
public:
    MemoryStressTest();
    ~MemoryStressTest();
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    TestResult getResult() const override;
    void setConfig(const TestConfig& config) override;

private:
    void memoryStressLoop();
    std::vector<std::thread> workerThreads_;
    std::vector<std::vector<uint8_t>> memoryBlocks_;
    std::atomic<bool> stopRequested_{false};
    std::mutex memoryMutex_;
};

class DiskStressTest : public StressTest {
public:
    DiskStressTest();
    ~DiskStressTest();
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    TestResult getResult() const override;
    void setConfig(const TestConfig& config) override;

private:
    void diskStressLoop();
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> stopRequested_{false};
    std::string tempDir_;
    std::vector<std::string> tempFiles_;
};

class GpuStressTest : public StressTest {
public:
    GpuStressTest();
    ~GpuStressTest();
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    TestResult getResult() const override;
    void setConfig(const TestConfig& config) override;

private:
    void gpuStressLoop();
    std::vector<std::thread> workerThreads_;
    std::atomic<bool> stopRequested_{false};
};

class StressTester {
public:
    StressTester();
    ~StressTester();
    
    void addTest(std::unique_ptr<StressTest> test);
    void runTest(const std::string& testName);
    void runAllTests();
    void stopAllTests();
    
    void setTestConfig(const std::string& testName, const TestConfig& config);
    void setGlobalDuration(std::chrono::milliseconds duration);
    void setGlobalIntensity(int intensity);
    
    std::vector<TestResult> getTestResults() const;
    TestResult getTestResult(const std::string& testName) const;
    bool isAnyTestRunning() const;
    
    void setSystemMonitor(std::shared_ptr<SystemMonitor> monitor);
    void enableMetricsLogging(bool enable);
    
    void setTestStartCallback(std::function<void(const std::string&)> callback);
    void setTestCompleteCallback(std::function<void(const TestResult&)> callback);

private:
    std::vector<std::unique_ptr<StressTest>> tests_;
    std::shared_ptr<SystemMonitor> systemMonitor_;
    std::atomic<bool> metricsLoggingEnabled_{true};
    
    std::function<void(const std::string&)> testStartCallback_;
    std::function<void(const TestResult&)> testCompleteCallback_;
    
    std::chrono::milliseconds globalDuration_{std::chrono::minutes(5)};
    int globalIntensity_{5};
};

} // namespace hwstress 