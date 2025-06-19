#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <memory>

namespace hwstress {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance();
    
    void setLogFile(const std::string& filename);
    void setLogLevel(LogLevel level);
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    // Stress test specific logging
    void logStressTest(const std::string& testName, const std::string& status, 
                      const std::string& details = "");
    void logSystemMetrics(double cpuUsage, double memoryUsage, double temperature);
    void logFaultInjection(const std::string& faultType, const std::string& target, 
                          bool success);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::string levelToString(LogLevel level);
    std::string getTimestamp();
    
    std::ofstream logFile_;
    LogLevel currentLevel_ = LogLevel::INFO;
    std::mutex logMutex_;
    bool consoleOutput_ = true;
};

#define LOGGER Logger::getInstance()

} // namespace hwstress 