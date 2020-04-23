#include "fileUtils.h"

#include "log.h"

#ifdef _MSC_VER
#include <direct.h>

bool FileUtils::doesFileExist(const std::string& fileName){
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

bool FileUtils::doesFileExist(const std::string& fileName){
    return fs::exists(fileName);
}

#endif


void FileUtils::warnIfFileExists(const std::string& fileName){
    if(FileUtils::doesFileExist(fileName)){
        Log::warn("File already exists: %s", fileName.c_str());
    }
}