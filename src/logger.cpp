#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "logger.hpp"
#include "config.hpp"

LogLevel Log::logLevel = LogLevel::LOG_FULL;

void Log::Init(LogLevel level)
{
	logLevel = level;

	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto tm = *std::localtime(&time);

	std::ostringstream oss;
	oss << "[" << std::put_time(&tm, "%d/%m/%Y - %H:%M:%S") << "] -> [PLUGIN] samp-node plugin started...\n";

	std::ofstream file("samp-node.log", std::ofstream::out | std::ofstream::app);
	if (file.is_open())
	{
		file << oss.str();
		file.close();
	}
}

std::ostringstream &Log::Get(LogLevel level)
{
	currentLevel = level;
	if (logLevel > level)
	{
		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);
		auto tm = *std::localtime(&time);

		os << "[" << std::put_time(&tm, "%d/%m/%Y - %H:%M:%S") << "]";
		os << " -> " << GetLevelName(level) << ": ";
		return os;
	}
	else
	{
		os.str("");
		return os;
	}
}

Log::Log()
{
}

Log::~Log()
{
	if (logLevel > currentLevel)
	{
		os << std::endl;

		std::ofstream file("samp-node.log", std::ofstream::out | std::ofstream::app);
		if (file.is_open())
		{
			file << os.str();
			std::cout << os.str();
			os.str("");
			os.clear();
		}
	}
}

std::string Log::GetLevelName(LogLevel level)
{
	switch (level)
	{
	case LogLevel::LOG_ERROR:
		return "[ERROR]";
	case LogLevel::LOG_WARN:
		return "[WARNING]";
	case LogLevel::LOG_INFO:
		return "[INFO]";
	case LogLevel::LOG_DEBUG:
		return "[DEBUG]";
	default:
		return "[LOG_UNKNOWN]";
	}
}