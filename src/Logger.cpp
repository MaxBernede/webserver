#include "Logger.hpp"

std::ofstream	Logger::logFile;
LogLevel		Logger::logLevel = INFO;
std::mutex		Logger::mtx;

void Logger::log(const std::string& message, LogLevel level) {
    if (level < logLevel) {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);

    std::string colorCode;
    switch (level) {
        case INFO:    colorCode = BLUE; break;
        case WARNING: colorCode = YELLOW; break;
        case ERROR:   colorCode = RED; break;
        case DEBUG:   colorCode = WHITE; break;
        default:      colorCode = WHITE; break;
    }

    std::string logMessage = getTimestamp() + " [" + levelToString(level) + "] " + message;
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;
    } else {
        std::cout << colorCode << logMessage << RESET << std::endl;
    }
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mtx);

    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open(filename, std::ios::app);
}

void Logger::setLogLevel(LogLevel level) {
    logLevel = level;
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_r(&in_time_t, &buf);

    std::stringstream ss;
    ss << std::put_time(&buf, "%H:%M:%S");
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case INFO:    return "INFO";
        case WARNING: return "WARNING";
        case ERROR:   return "ERROR";
        case DEBUG:   return "DEBUG";
        default:      return "UNKNOWN";
    }
}
