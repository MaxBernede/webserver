#pragma once
#include <iostream>
#include <sstream>

// ANSI color escape sequences
const std::string RED = "\033[1;31m";
const std::string GREEN = "\033[1;32m";
const std::string YELLOW = "\033[1;33m";
const std::string BLUE = "\033[1;34m";
const std::string MAGENTA = "\033[1;35m";
const std::string CYAN = "\033[1;36m";
const std::string RESET = "\033[0m";

template<typename T>
void printColor(const std::string& color, T&& arg) {
    std::cout << color << std::forward<T>(arg) << std::endl << RESET;
}

template<typename T, typename... Args>
void printColor(const std::string& color, T&& arg, Args&&... args) {
    std::cout << color << std::forward<T>(arg);
    printColor(color, std::forward<Args>(args)...);
}
