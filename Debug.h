#pragma once

#include <vector>
#include <string>
#include <chrono>

enum LogSource { Vulkan, Engine, Game };
enum LogType { Message, Warning, Error };

struct LogEntry {
    std::string message;
    std::string logSource;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    LogType logType;
};

struct Debug {
    void log(std::string message, LogSource source = Engine, LogType type = Message);
    const std::vector<LogEntry>& getLogEntries();
    std::vector<LogEntry> logEntries;
    int logCount = 0;
};

std::string enumToString(LogSource source);

static Debug debug;