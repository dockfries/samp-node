#include "logger.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "config.hpp"

LogLevel Log::logLevel = LogLevel::LOG_FULL;
std::string Log::timeFormat = "%Y-%m-%dT%H:%M:%S%z";

void Log::Init(LogLevel level, const std::string &timeFormat) {
  logLevel = level;

  if (!timeFormat.empty())
    Log::timeFormat = timeFormat;

  L_INFO << "[PLUGIN] samp-node plugin started...";
}

std::ostringstream &Log::Get(LogLevel level) {
  currentLevel = level;
  if (logLevel > level) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time);

    os << "[" << std::put_time(&tm, timeFormat.c_str()) << "]";
    os << " " << GetLevelName(level) << " ";
    return os;
  } else {
    os.str("");
    return os;
  }
}

Log::Log() {}

Log::~Log() {
  if (logLevel > currentLevel) {
    os << std::endl;

    std::ofstream file("samp-node.log",
                       std::ofstream::out | std::ofstream::app);
    if (file.is_open()) {
      file << os.str();
      std::cout << os.str();
      os.str("");
      os.clear();
    }
  }
}

std::string Log::GetLevelName(LogLevel level) {
  switch (level) {
  case LogLevel::LOG_ERROR:
    return "[Error]";
  case LogLevel::LOG_WARN:
    return "[Warning]";
  case LogLevel::LOG_INFO:
    return "[Info]";
  case LogLevel::LOG_DEBUG:
    return "[Debug]";
  default:
    return "[Unknown]";
  }
}