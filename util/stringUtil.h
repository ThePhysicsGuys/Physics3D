#pragma once

#include <vector>
#include <string>

std::vector<std::string> split(const std::string& string, char splitter);

bool startWith(std::string string, std::string prefix);

std::string ltrim(std::string string);

std::string rtrim(std::string string);

std::string trim(std::string string);