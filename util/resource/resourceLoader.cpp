#include "resourceLoader.h"

#include "resourceDescriptor.h"

#include "../log.h"

#ifdef _WIN32

#include <windows.h>

class ResourceLoader {
public:
	struct Parameters {
		std::size_t size = 0;
		void* pointer = nullptr;
	};

private:
	HRSRC hResource = nullptr;
	HGLOBAL hMemory = nullptr;

	Parameters parameters;

public:
	ResourceLoader(int id, const std::string& type) {
		hResource = FindResource(nullptr, MAKEINTRESOURCEA(id), type.c_str());
		hMemory = LoadResource(nullptr, hResource);

		parameters.size = SizeofResource(nullptr, hResource);
		parameters.pointer = LockResource(hMemory);
	}

	std::string getResourceString() const {
		std::string string;
		if (parameters.pointer != nullptr)
			string = std::string(reinterpret_cast<char*>(parameters.pointer), parameters.size);
		return string;
	}
};

std::string getResourceAsString(const ResourceDescriptor* list, int id) {
	ResourceDescriptor resource = list[id];
	ResourceLoader manager(resource.id, resource.type);
	return manager.getResourceString();
}
#else

#include <string>
#include <fstream>
#include <streambuf>

std::string getResourceAsString(const ResourceDescriptor* list, int id) {
	const ResourceDescriptor& resource = list[id];
	std::ifstream file;
	file.open(resource.fileName, std::ios::in);
	if(!file) {
		Log::error("Could not open %s", resource.fileName);
		throw std::runtime_error("Could not open file!");
	}
	return std::string(std::istreambuf_iterator<char>(file),
					   std::istreambuf_iterator<char>());
}

#endif

