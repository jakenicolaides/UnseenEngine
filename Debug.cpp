#include "Debug.h"
#include <vector>
#include <string>
#include <chrono>

void Debug::log(std::string message, LogSource source, LogType type)
{
    LogEntry newLogEntry;
    newLogEntry.message = message;
    newLogEntry.logSource = enumToString(source),
    newLogEntry.timestamp = std::chrono::system_clock::now();
    newLogEntry.logType = type;
    logEntries.push_back(newLogEntry);
    return;
}

const std::vector<LogEntry>& Debug::getLogEntries()
{
    return logEntries;
}

std::string enumToString(LogSource source)
{
    switch (source) {
    case Vulkan:
        return "Vulkan";
    case Engine:
        return "Engine";
    case Game:
        return "Game";
    }
}


