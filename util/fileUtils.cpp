#include "fileUtils.h"

#include "log.h"
#include <fstream>
#include <sstream>

#ifdef _MSC_VER
	#include <direct.h>

	bool Util::doesFileExist(const std::string& fileName) {
		struct stat buffer;
		if (stat(fileName.c_str(), &buffer) != -1) {
			return true;
		}
		return false;
	}

#else
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
		Log::print(Log::Color::ERROR, "Fail");
		Log::setDelimiter("\n");

		return "";
	}

	Log::print(Log::Color::DEBUG, "Done");
	Log::setDelimiter("\n");

	std::string line;
	std::stringstream stringStream;
	while (getline(fileStream, line))
		stringStream << line << "\n";

	fileStream.close();

	return stringStream.str();
}

};