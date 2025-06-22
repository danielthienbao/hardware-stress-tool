#include "stress_tester.h"
#include "logger.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>
#include <cmath>

StressTester::StressTester() : testRunning(false) {}

StressTester::~StressTester() {
    stopTest();
}

void StressTester::startTest(const StressTestConfig& config) {
    if (testRunning) {
        Logger::getInstance().log(LogLevel::WARNING, "Test already running");
        return;
    }

    currentConfig = config;
    currentResults = StressTestResults{};
    currentResults.testType = config.type;
    testRunning = true;
    testStartTime = std::chrono::steady_clock::now();

    int numThreads = config.threadCount > 0 ? config.threadCount : std::thread::hardware_concurrency();
    
    Logger::getInstance().log(LogLevel::INFO, 
        "Starting stress test - Type: " + std::to_string(static_cast<int>(config.type)) + 
        ", Duration: " + std::to_string(config.durationSeconds) + "s, Threads: " + std::to_string(numThreads));

    switch (config.type) {
        case StressType::CPU:
            runCpuStress();
            break;
        case StressType::MEMORY:
            runMemoryStress();
            break;
        case StressType::DISK:
            runDiskStress();
            break;
        case StressType::GPU_SIMULATION:
            runGpuSimulationStress();
            break;
        case StressType::COMBINED:
            runCpuStress();
            runMemoryStress();
            break;
    }
}

void StressTester::stopTest() {
    if (!testRunning) {
        return;
    }

    testRunning = false;
    
    for (auto& thread : testThreads) {
        if (thread && thread->joinable()) {
            thread->join();
        }
    }
    testThreads.clear();

    auto endTime = std::chrono::steady_clock::now();
    currentResults.actualDuration = endTime - testStartTime;
    currentResults.testPassed = (currentResults.errorsEncountered == 0);
    
    Logger::getInstance().log(LogLevel::INFO, "Stress test completed");
    
    if (resultCallback) {
        resultCallback(currentResults);
    }
}

StressTestResults StressTester::getResults() const {
    return currentResults;
}

void StressTester::setProgressCallback(std::function<void(double)> callback) {
    progressCallback = callback;
}

void StressTester::setResultCallback(std::function<void(const StressTestResults&)> callback) {
    resultCallback = callback;
}

void StressTester::runCpuStress() {
    int numThreads = currentConfig.threadCount > 0 ? currentConfig.threadCount : std::thread::hardware_concurrency();
    
    for (int i = 0; i < numThreads; ++i) {
        testThreads.push_back(std::make_unique<std::thread>(&StressTester::cpuWorkerThread, this, i));
    }
}

void StressTester::runMemoryStress() {
    int numThreads = std::min(4, static_cast<int>(std::thread::hardware_concurrency()));
    
    for (int i = 0; i < numThreads; ++i) {
        testThreads.push_back(std::make_unique<std::thread>(&StressTester::memoryWorkerThread, this, i));
    }
}

void StressTester::runDiskStress() {
    int numThreads = 2; // Limit disk threads to avoid overwhelming I/O
    
    for (int i = 0; i < numThreads; ++i) {
        testThreads.push_back(std::make_unique<std::thread>(&StressTester::diskWorkerThread, this, i));
    }
}

void StressTester::runGpuSimulationStress() {
    int numThreads = std::thread::hardware_concurrency();
    
    for (int i = 0; i < numThreads; ++i) {
        testThreads.push_back(std::make_unique<std::thread>(&StressTester::gpuSimWorkerThread, this, i));
    }
}

void StressTester::cpuWorkerThread(int threadId) {
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(currentConfig.durationSeconds);
    
    size_t operations = 0;
    double totalResult = 0.0;
    
    while (testRunning && std::chrono::steady_clock::now() < endTime) {
        // Perform CPU-intensive calculations
        totalResult += calculateCpuIntensive();
        operations++;
        
        if (operations % 1000 == 0) {
            auto now = std::chrono::steady_clock::now();
            double progress = std::chrono::duration<double>(now - startTime).count() / currentConfig.durationSeconds;
            updateProgress(progress);
        }
    }
    
    currentResults.operationsCompleted += operations;
    Logger::getInstance().log(LogLevel::DEBUG, 
        "CPU thread " + std::to_string(threadId) + " completed " + std::to_string(operations) + " operations");
}

void StressTester::memoryWorkerThread(int threadId) {
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(currentConfig.durationSeconds);
    
    size_t blockSize = currentConfig.memorySize / 4; // Divide memory among threads
    size_t operations = 0;
    
    try {
        while (testRunning && std::chrono::steady_clock::now() < endTime) {
            // Allocate memory block
            std::vector<char> memBlock(blockSize);
            
            // Fill with random data
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            
            for (size_t i = 0; i < blockSize; ++i) {
                memBlock[i] = static_cast<char>(dis(gen));
            }
            
            // Read back and verify
            volatile char sum = 0;
            for (size_t i = 0; i < blockSize; ++i) {
                sum += memBlock[i];
            }
            
            operations++;
            
            if (operations % 10 == 0) {
                auto now = std::chrono::steady_clock::now();
                double progress = std::chrono::duration<double>(now - startTime).count() / currentConfig.durationSeconds;
                updateProgress(progress);
            }
        }
    } catch (const std::exception& e) {
        currentResults.errorsEncountered++;
        Logger::getInstance().log(LogLevel::ERROR, 
            "Memory stress error in thread " + std::to_string(threadId) + ": " + e.what());
    }
    
    currentResults.operationsCompleted += operations;
}

void StressTester::diskWorkerThread(int threadId) {
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(currentConfig.durationSeconds);
    
    std::string filename = currentConfig.diskPath + "_thread_" + std::to_string(threadId);
    size_t operations = 0;
    const size_t blockSize = 1024 * 1024; // 1MB blocks
    
    try {
        std::vector<char> data(blockSize, 'A' + threadId);
        
        while (testRunning && std::chrono::steady_clock::now() < endTime) {
            // Write test
            {
                std::ofstream file(filename, std::ios::binary);
                file.write(data.data(), data.size());
                file.flush();
            }
            
            // Read test
            {
                std::ifstream file(filename, std::ios::binary);
                std::vector<char> readData(blockSize);
                file.read(readData.data(), readData.size());
                
                // Verify data integrity
                if (readData != data) {
                    currentResults.errorsEncountered++;
                }
            }
            
            operations++;
            
            if (operations % 10 == 0) {
                auto now = std::chrono::steady_clock::now();
                double progress = std::chrono::duration<double>(now - startTime).count() / currentConfig.durationSeconds;
                updateProgress(progress);
            }
        }
        
        // Clean up test file
        std::remove(filename.c_str());
        
    } catch (const std::exception& e) {
        currentResults.errorsEncountered++;
        Logger::getInstance().log(LogLevel::ERROR, 
            "Disk stress error in thread " + std::to_string(threadId) + ": " + e.what());
    }
    
    currentResults.operationsCompleted += operations;
}

void StressTester::gpuSimWorkerThread(int threadId) {
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(currentConfig.durationSeconds);
    
    size_t operations = 0;
    const size_t matrixSize = 128;
    
    // Simulate GPU-like parallel computations
    std::vector<std::vector<double>> matrixA(matrixSize, std::vector<double>(matrixSize));
    std::vector<std::vector<double>> matrixB(matrixSize, std::vector<double>(matrixSize));
    std::vector<std::vector<double>> result(matrixSize, std::vector<double>(matrixSize, 0.0));
    
    // Initialize matrices
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (size_t i = 0; i < matrixSize; ++i) {
        for (size_t j = 0; j < matrixSize; ++j) {
            matrixA[i][j] = dis(gen);
            matrixB[i][j] = dis(gen);
        }
    }
    
    while (testRunning && std::chrono::steady_clock::now() < endTime) {
        // Matrix multiplication (simulating GPU compute)
        for (size_t i = 0; i < matrixSize; ++i) {
            for (size_t j = 0; j < matrixSize; ++j) {
                result[i][j] = 0.0;
                for (size_t k = 0; k < matrixSize; ++k) {
                    result[i][j] += matrixA[i][k] * matrixB[k][j];
                }
            }
        }
        
        operations++;
        
        if (operations % 5 == 0) {
            auto now = std::chrono::steady_clock::now();
            double progress = std::chrono::duration<double>(now - startTime).count() / currentConfig.durationSeconds;
            updateProgress(progress);
        }
    }
    
    currentResults.operationsCompleted += operations;
}

void StressTester::updateProgress(double progress) {
    if (progressCallback) {
        progressCallback(std::min(1.0, progress));
    }
}

void StressTester::updateResults() {
    // This method can be used to update results periodically during test execution
}

double StressTester::calculateCpuIntensive() {
    // CPU-intensive mathematical operations
    double result = 0.0;
    for (int i = 0; i < 10000; ++i) {
        result += std::sin(i) * std::cos(i) * std::sqrt(i + 1);
        result = std::fmod(result, 1000000.0); // Prevent overflow
    }
    return result;
} 