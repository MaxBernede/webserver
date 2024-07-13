#pragma once
#include <iostream>
#include <sstream>

template<typename T>
void printColor(const std::string& color, T&& arg) {
    std::cout << color << std::forward<T>(arg) << std::endl << RESET;
}

template<typename T, typename... Args>
void printColor(const std::string& color, T&& arg, Args&&... args) {
    std::cout << color << std::forward<T>(arg);
    printColor(color, std::forward<Args>(args)...);
}
