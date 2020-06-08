#pragma once

#include <string>

namespace FileUtils {

bool doesFileExist(const std::string& fileName);
void warnIfFileExists(const std::string& fileName);

};