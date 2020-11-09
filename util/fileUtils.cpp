#include "fileUtils.h"

#include "log.h"
#include <fstream>
#include <sstream>

#ifdef _WIN32
	#include <direct.h>
	#include <Windows.h>

	bool Util::doesFileExist(const std::string& fileName) {
		struct stat buffer;
		if (stat(fileName.c_str(), &buffer) != -1) {
			return true;
		}
		return false;
	}
	
	std::string Util::getFullPath(const std::string& fileName) {
		TCHAR buf[MAX_PATH] = TEXT("");
		TCHAR** lppPart = {NULL};
		GetFullPathName(fileName.c_str(), MAX_PATH, buf, lppPart);
		return std::string(buf);
	}

#else
	#include <stdlib.h>
	// for some reason gcc still does not support <filesystem>
	#if __GNUC__ >= 8
	#include <filesystem>
	namespace fs = std::filesystem;
	#else
	#include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
	#endif

	bool Util::doesFileExist(const std::string& fileName) {
		return fs::exists(fileName);
	}

	std::string Util::getFullPath(const std::string& fileName) {
		char* path = realpath(fileName.c_str(), NULL);
		std::string result(path);
		free(path);
		return result;
	}
#endif

namespace Util {

void warnIfFileExists(const std::string& fileName) {
	if (doesFileExist(fileName)) {
		Log::warn("File already exists: %s", fileName.c_str());
	}
}

std::string parseFile(const std::string& path) {
	if (path.empty())
		return std::string();

	std::ifstream fileStream(path);

	Log::setDelimiter("");
	Log::info("Reading (%s) ... ", path.c_str());

	if (fileStream.fail() || !fileStream.is_open()) {
		Log::error("Fail");
		Log::setDelimiter("\n");

		return "";
	}

	Log::debug("Done");
	Log::setDelimiter("\n");

	std::string line;
	std::stringstream stringStream;
	while (getline(fileStream, line))
		stringStream << line << "\n";

	fileStream.close();

	return stringStream.str();
}

};