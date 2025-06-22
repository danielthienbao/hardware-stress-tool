#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();
    void initialize(const std::string& filename = "stress_tool.log");
    void log(LogLevel level, const std::string& message);
    void setLogLevel(LogLevel level);
    void enableConsoleOutput(bool enable);

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile;
    std::mutex logMutex;
    LogLevel currentLogLevel = LogLevel::INFO;
    bool consoleOutput = true;
    std::string getTimestamp();
    std::string levelToString(LogLevel level);
};