#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <mutex>
#include "Colors.hpp"


enum LogLevel {
    INFO,			//Blue
    WARNING,		//Yellow
    ERROR,			//Red
    DEBUG			//White
};

class Logger {
public:
    static void log(const std::string& message, LogLevel level = INFO);

    static void setLogFile(const std::string& filename);
    static void setLogLevel(LogLevel level);

private:
    Logger() = default;

    static std::string getTimestamp();
    static std::string levelToString(LogLevel level);

    static std::ofstream logFile;
    static LogLevel logLevel;
    static std::mutex mtx;
};
