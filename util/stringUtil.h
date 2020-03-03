#pragma once

#include <vector>
#include <string>

std::vector<std::string> split(const std::string& string, char splitter);

bool startWith(const std::string& string, const std::string& prefix);
bool endsWith(const std::string& string, const std::string& suffix);

std::string ltrim(std::string string);
std::string rtrim(std::string string);
std::string trim(std::string string);