#include "stress_tester.h"
#include "logger.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <random>
#include <fstream>
#include <filesystem>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#endif

namespace hwstress {

// CPU Stress Test Implementation
CpuStressTest::CpuStressTest() {
    config_.type = TestType::CPU_STRESS;
    config_.name = "CPU Stress Test";
    result_.type = TestType::CPU_STRESS;
    result_.name = config_.name;
}

CpuStressTest::~CpuStressTest() {
    stop();
}

void CpuStressTest::start() {
    if (running_.load()) {
        return;
    }
    
    running_ = true;
    stopRequested_ = false;
    startTime_ = std::chrono::system_clock::now();
    result_.status = TestStatus::RUNNING;
    result_.errorMessage.clear();
    
    // Determine number of threads based on intensity
    int threadCount = std::thread::hardware_concurrency();
    threadCount = std::max(1, std::min(threadCount, config_.intensity * 2));
    
    LOGGER.info("Starting CPU stress test with " + std::to_string(threadCount) + " threads");
    
    // Start worker threads
    for (int i = 0; i < threadCount; ++i) {
        workerThreads_.emplace_back(&CpuStressTest::cpuStressLoop, this);
    }
}

void CpuStressTest::stop() {
    if (!running_.load()) {
        return;
    }
    
    stopRequested_ = true;
    running_ = false;
    
    // Wait for all threads to finish
    for (auto& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    workerThreads_.clear();
    
    auto endTime = std::chrono::system_clock::now();
    result_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
    result_.status = TestStatus::COMPLETED;
    
    LOGGER.info("CPU stress test completed in " + std::to_string(result_.duration.count()) + "ms");
}

bool CpuStressTest::isRunning() const {
    return running_.load();
}

TestResult CpuStressTest::getResult() const {
    return result_;
}

void CpuStressTest::setConfig(const TestConfig& config) {
    config_ = config;
    result_.name = config.name;
}

void CpuStressTest::cpuStressLoop() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    while (!stopRequested_.load()) {
        // Perform CPU-intensive calculations
        double sum = 0.0;
        for (int i = 0; i < 10000 && !stopRequested_.load(); ++i) {
            sum += std::sin(dis(gen)) * std::cos(dis(gen));
        }
        
        // Prevent compiler optimization
        if (sum > 1000000.0) {
            LOGGER.debug("CPU stress calculation result: " + std::to_string(sum));
        }
    }
}

// Memory Stress Test Implementation
MemoryStressTest::MemoryStressTest() {
    config_.type = TestType::MEMORY_STRESS;
    config_.name = "Memory Stress Test";
    result_.type = TestType::MEMORY_STRESS;
    result_.name = config_.name;
}

MemoryStressTest::~MemoryStressTest() {
    stop();
}

void MemoryStressTest::start() {
    if (running_.load()) {
        return;
    }
    
    running_ = true;
    stopRequested_ = false;
    startTime_ = std::chrono::system_clock::now();
    result_.status = TestStatus::RUNNING;
    result_.errorMessage.clear();
    
    // Determine memory allocation size based on intensity
    size_t blockSize = 1024 * 1024; // 1MB base
    size_t totalSize = blockSize * config_.intensity * 10; // Scale with intensity
    
    LOGGER.info("Starting memory stress test with " + std::to_string(totalSize / (1024*1024)) + "MB");
    
    // Start worker threads
    int threadCount = std::min(4, config_.intensity);
    for (int i = 0; i < threadCount; ++i) {
        workerThreads_.emplace_back(&MemoryStressTest::memoryStressLoop, this);
    }
}

void MemoryStressTest::stop() {
    if (!running_.load()) {
        return;
    }
    
    stopRequested_ = true;
    running_ = false;
    
    // Wait for all threads to finish
    for (auto& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    workerThreads_.clear();
    
    // Clear memory blocks
    {
        std::lock_guard<std::mutex> lock(memoryMutex_);
        memoryBlocks_.clear();
    }
    
    auto endTime = std::chrono::system_clock::now();
    result_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
    result_.status = TestStatus::COMPLETED;
    
    LOGGER.info("Memory stress test completed in " + std::to_string(result_.duration.count()) + "ms");
}

bool MemoryStressTest::isRunning() const {
    return running_.load();
}

TestResult MemoryStressTest::getResult() const {
    return result_;
}

void MemoryStressTest::setConfig(const TestConfig& config) {
    config_ = config;
    result_.name = config.name;
}

void MemoryStressTest::memoryStressLoop() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sizeDis(1024, 1024 * 1024); // 1KB to 1MB
    std::uniform_int_distribution<> indexDis(0, 255);
    
    while (!stopRequested_.load()) {
        try {
            // Allocate memory block
            size_t blockSize = sizeDis(gen);
            std::vector<uint8_t> block(blockSize);
            
            // Fill with random data
            for (auto& byte : block) {
                byte = indexDis(gen);
            }
            
            // Store block
            {
                std::lock_guard<std::mutex> lock(memoryMutex_);
                memoryBlocks_.push_back(std::move(block));
            }
            
            // Simulate memory access patterns
            if (!memoryBlocks_.empty()) {
                std::lock_guard<std::mutex> lock(memoryMutex_);
                for (auto& block : memoryBlocks_) {
                    if (!block.empty()) {
                        block[0] = indexDis(gen);
                    }
                }
            }
            
            // Limit memory usage
            {
                std::lock_guard<std::mutex> lock(memoryMutex_);
                if (memoryBlocks_.size() > 100) {
                    memoryBlocks_.erase(memoryBlocks_.begin());
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
        } catch (const std::exception& e) {
            LOGGER.error("Memory stress test error: " + std::string(e.what()));
            break;
        }
    }
}

// Disk Stress Test Implementation
DiskStressTest::DiskStressTest() {
    config_.type = TestType::DISK_STRESS;
    config_.name = "Disk Stress Test";
    result_.type = TestType::DISK_STRESS;
    result_.name = config_.name;
    
    // Create temporary directory
    tempDir_ = std::filesystem::temp_directory_path().string() + "/hwstress_test";
    std::filesystem::create_directories(tempDir_);
}

DiskStressTest::~DiskStressTest() {
    stop();
    // Clean up temporary files
    for (const auto& file : tempFiles_) {
        std::filesystem::remove(file);
    }
    std::filesystem::remove_all(tempDir_);
}

void DiskStressTest::start() {
    if (running_.load()) {
        return;
    }
    
    running_ = true;
    stopRequested_ = false;
    startTime_ = std::chrono::system_clock::now();
    result_.status = TestStatus::RUNNING;
    result_.errorMessage.clear();
    
    LOGGER.info("Starting disk stress test");
    
    // Start worker threads
    int threadCount = std::min(2, config_.intensity / 2);
    for (int i = 0; i < threadCount; ++i) {
        workerThreads_.emplace_back(&DiskStressTest::diskStressLoop, this);
    }
}

void DiskStressTest::stop() {
    if (!running_.load()) {
        return;
    }
    
    stopRequested_ = true;
    running_ = false;
    
    // Wait for all threads to finish
    for (auto& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    workerThreads_.clear();
    
    auto endTime = std::chrono::system_clock::now();
    result_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
    result_.status = TestStatus::COMPLETED;
    
    LOGGER.info("Disk stress test completed in " + std::to_string(result_.duration.count()) + "ms");
}

bool DiskStressTest::isRunning() const {
    return running_.load();
}

TestResult DiskStressTest::getResult() const {
    return result_;
}

void DiskStressTest::setConfig(const TestConfig& config) {
    config_ = config;
    result_.name = config.name;
}

void DiskStressTest::diskStressLoop() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sizeDis(1024, 1024 * 1024); // 1KB to 1MB
    std::uniform_int_distribution<> charDis(32, 126); // Printable ASCII
    
    int fileCounter = 0;
    
    while (!stopRequested_.load()) {
        try {
            // Create temporary file
            std::string filename = tempDir_ + "/stress_test_" + std::to_string(fileCounter++) + ".tmp";
            std::ofstream file(filename, std::ios::binary);
            
            if (file.is_open()) {
                // Write random data
                size_t fileSize = sizeDis(gen);
                std::vector<char> buffer(fileSize);
                
                for (auto& c : buffer) {
                    c = static_cast<char>(charDis(gen));
                }
                
                file.write(buffer.data(), buffer.size());
                file.close();
                
                tempFiles_.push_back(filename);
                
                // Read back and verify
                std::ifstream readFile(filename, std::ios::binary);
                if (readFile.is_open()) {
                    readFile.seekg(0, std::ios::end);
                    size_t readSize = readFile.tellg();
                    readFile.close();
                    
                    if (readSize != fileSize) {
                        LOGGER.warning("File size mismatch in disk stress test");
                    }
                }
                
                // Limit number of files
                if (tempFiles_.size() > 50) {
                    std::filesystem::remove(tempFiles_.front());
                    tempFiles_.erase(tempFiles_.begin());
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
        } catch (const std::exception& e) {
            LOGGER.error("Disk stress test error: " + std::string(e.what()));
            break;
        }
    }
}

// GPU Stress Test Implementation (Basic simulation)
GpuStressTest::GpuStressTest() {
    config_.type = TestType::GPU_STRESS;
    config_.name = "GPU Stress Test";
    result_.type = TestType::GPU_STRESS;
    result_.name = config_.name;
}

GpuStressTest::~GpuStressTest() {
    stop();
}

void GpuStressTest::start() {
    if (running_.load()) {
        return;
    }
    
    running_ = true;
    stopRequested_ = false;
    startTime_ = std::chrono::system_clock::now();
    result_.status = TestStatus::RUNNING;
    result_.errorMessage.clear();
    
    LOGGER.info("Starting GPU stress test (simulated)");
    
    // Start worker threads (simulating GPU work)
    int threadCount = std::min(4, config_.intensity);
    for (int i = 0; i < threadCount; ++i) {
        workerThreads_.emplace_back(&GpuStressTest::gpuStressLoop, this);
    }
}

void GpuStressTest::stop() {
    if (!running_.load()) {
        return;
    }
    
    stopRequested_ = true;
    running_ = false;
    
    // Wait for all threads to finish
    for (auto& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    workerThreads_.clear();
    
    auto endTime = std::chrono::system_clock::now();
    result_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
    result_.status = TestStatus::COMPLETED;
    
    LOGGER.info("GPU stress test completed in " + std::to_string(result_.duration.count()) + "ms");
}

bool GpuStressTest::isRunning() const {
    return running_.load();
}

TestResult GpuStressTest::getResult() const {
    return result_;
}

void GpuStressTest::setConfig(const TestConfig& config) {
    config_ = config;
    result_.name = config.name;
}

void GpuStressTest::gpuStressLoop() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    while (!stopRequested_.load()) {
        // Simulate GPU-intensive work (matrix operations)
        const int matrixSize = 100;
        std::vector<std::vector<double>> matrixA(matrixSize, std::vector<double>(matrixSize));
        std::vector<std::vector<double>> matrixB(matrixSize, std::vector<double>(matrixSize));
        std::vector<std::vector<double>> result(matrixSize, std::vector<double>(matrixSize));
        
        // Fill matrices with random data
        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                matrixA[i][j] = dis(gen);
                matrixB[i][j] = dis(gen);
            }
        }
        
        // Perform matrix multiplication (simulating GPU work)
        for (int i = 0; i < matrixSize && !stopRequested_.load(); ++i) {
            for (int j = 0; j < matrixSize && !stopRequested_.load(); ++j) {
                result[i][j] = 0.0;
                for (int k = 0; k < matrixSize; ++k) {
                    result[i][j] += matrixA[i][k] * matrixB[k][j];
                }
            }
        }
        
        // Prevent compiler optimization
        if (result[0][0] > 1000000.0) {
            LOGGER.debug("GPU stress calculation result: " + std::to_string(result[0][0]));
        }
    }
}

// StressTester Implementation
StressTester::StressTester() {
    LOGGER.info("StressTester initialized");
}

StressTester::~StressTester() {
    stopAllTests();
}

void StressTester::addTest(std::unique_ptr<StressTest> test) {
    tests_.push_back(std::move(test));
    LOGGER.info("Test added to stress tester");
}

void StressTester::runTest(const std::string& testName) {
    LOGGER.info("Running test: " + testName);
    
    for (auto& test : tests_) {
        if (test->getResult().name == testName) {
            if (testStartCallback_) {
                testStartCallback_(testName);
            }
            
            test->setConfig({test->getResult().type, testName, globalDuration_, globalIntensity_, true, {}});
            test->start();
            
            // Wait for test to complete
            while (test->isRunning()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            if (testCompleteCallback_) {
                testCompleteCallback_(test->getResult());
            }
            break;
        }
    }
}

void StressTester::runAllTests() {
    LOGGER.info("Running all stress tests");
    
    for (auto& test : tests_) {
        if (testStartCallback_) {
            testStartCallback_(test->getResult().name);
        }
        
        test->setConfig({test->getResult().type, test->getResult().name, globalDuration_, globalIntensity_, true, {}});
        test->start();
    }
    
    // Wait for all tests to complete
    while (isAnyTestRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    for (auto& test : tests_) {
        if (testCompleteCallback_) {
            testCompleteCallback_(test->getResult());
        }
    }
}

void StressTester::stopAllTests() {
    LOGGER.info("Stopping all stress tests");
    
    for (auto& test : tests_) {
        if (test->isRunning()) {
            test->stop();
        }
    }
}

void StressTester::setTestConfig(const std::string& testName, const TestConfig& config) {
    LOGGER.info("Setting config for test: " + testName);
    
    for (auto& test : tests_) {
        if (test->getResult().name == testName) {
            test->setConfig(config);
            break;
        }
    }
}

void StressTester::setGlobalDuration(std::chrono::milliseconds duration) {
    globalDuration_ = duration;
    LOGGER.info("Global duration set to: " + std::to_string(duration.count()) + "ms");
}

void StressTester::setGlobalIntensity(int intensity) {
    globalIntensity_ = intensity;
    LOGGER.info("Global intensity set to: " + std::to_string(intensity));
}

std::vector<TestResult> StressTester::getTestResults() const {
    std::vector<TestResult> results;
    for (const auto& test : tests_) {
        results.push_back(test->getResult());
    }
    return results;
}

TestResult StressTester::getTestResult(const std::string& testName) const {
    for (const auto& test : tests_) {
        if (test->getResult().name == testName) {
            return test->getResult();
        }
    }
    return TestResult{};
}

bool StressTester::isAnyTestRunning() const {
    for (const auto& test : tests_) {
        if (test->isRunning()) {
            return true;
        }
    }
    return false;
}

void StressTester::setSystemMonitor(std::shared_ptr<SystemMonitor> monitor) {
    systemMonitor_ = monitor;
    LOGGER.info("System monitor set for stress tester");
}

void StressTester::enableMetricsLogging(bool enable) {
    metricsLoggingEnabled_.store(enable);
    LOGGER.info("Metrics logging " + std::string(enable ? "enabled" : "disabled"));
}

void StressTester::setTestStartCallback(std::function<void(const std::string&)> callback) {
    testStartCallback_ = callback;
}

void StressTester::setTestCompleteCallback(std::function<void(const TestResult&)> callback) {
    testCompleteCallback_ = callback;
}

} // namespace hwstress 