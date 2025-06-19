#include "logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace hwstress {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(filename, std::ios::app);
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel_ = level;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::string timestamp = getTimestamp();
    std::string levelStr = levelToString(level);
    
    std::string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    if (consoleOutput_) {
        std::cout << logMessage << std::endl;
    }
    
    if (logFile_.is_open()) {
        logFile_ << logMessage << std::endl;
        logFile_.flush();
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::logStressTest(const std::string& testName, const std::string& status, 
                          const std::string& details) {
    std::string message = "STRESS_TEST [" + testName + "] " + status;
    if (!details.empty()) {
        message += " - " + details;
    }
    info(message);
}

void Logger::logSystemMetrics(double cpuUsage, double memoryUsage, double temperature) {
    std::ostringstream oss;
    oss << "SYSTEM_METRICS CPU:" << std::fixed << std::setprecision(1) << cpuUsage 
        << "% MEM:" << memoryUsage << "% TEMP:" << temperature << "°C";
    debug(oss.str());
}

void Logger::logFaultInjection(const std::string& faultType, const std::string& target, 
                              bool success) {
    std::string status = success ? "SUCCESS" : "FAILED";
    std::string message = "FAULT_INJECTION [" + faultType + "] " + target + " - " + status;
    info(message);
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace hwstress 