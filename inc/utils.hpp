#pragma once

int genererPort();
std::string read_html_file(const std::string &filename);
bool endsWith(const std::string& str, const std::string& suffix);
void create_file(std::string const &content, std::string const &location);
std::string firstWord(const std::string& str);