#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <string>

enum class StressType {
    CPU,
    MEMORY,
    DISK,
    GPU_SIMULATION,
    COMBINED
};

struct StressTestConfig {
    StressType type = StressType::CPU;
    int durationSeconds = 60;
    int threadCount = 0; // 0 = auto-detect
    size_t memorySize = 1024 * 1024 * 100; // 100MB default
    std::string diskPath = "./stress_test_file.tmp";
    bool enableLogging = true;
    double targetLoad = 0.8; // 80% load target
};

struct StressTestResults {
    StressType testType;
    std::chrono::duration<double> actualDuration;
    double averageLoad = 0.0;
    double peakLoad = 0.0;
    size_t operationsCompleted = 0;
    size_t errorsEncountered = 0;
    bool testPassed = false;
    std::string additionalInfo;
};

class StressTester {
public:
    StressTester();
    ~StressTester();

    void startTest(const StressTestConfig& config);
    void stopTest();
    bool isRunning() const { return testRunning; }
    StressTestResults getResults() const;
    
    void setProgressCallback(std::function<void(double)> callback);
    void setResultCallback(std::function<void(const StressTestResults&)> callback);

private:
    std::atomic<bool> testRunning;
    std::vector<std::unique_ptr<std::thread>> testThreads;
    StressTestConfig currentConfig;
    StressTestResults currentResults;
    std::chrono::steady_clock::time_point testStartTime;
    
    std::function<void(double)> progressCallback;
    std::function<void(const StressTestResults&)> resultCallback;

    // Test implementations
    void runCpuStress();
    void runMemoryStress();
    void runDiskStress();
    void runGpuSimulationStress();
    
    // Helper functions
    void cpuWorkerThread(int threadId);
    void memoryWorkerThread(int threadId);
    void diskWorkerThread(int threadId);
    void gpuSimWorkerThread(int threadId);
    
    void updateProgress(double progress);
    void updateResults();
    double calculateCpuIntensive();
}; 