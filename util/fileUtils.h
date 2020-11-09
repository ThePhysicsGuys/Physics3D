#pragma once

#include <string>

namespace Util {

bool doesFileExist(const std::string& fileName);
void warnIfFileExists(const std::string& fileName);
std::string parseFile(const std::string& path);
std::string getFullPath(const std::string& filename);

};