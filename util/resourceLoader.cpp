#include "resourceLoader.h"

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
	ResourceLoader(int id, const std::string &type) {
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

std::string getResourceAsString(ResourceList list, int id) {
	ResourceStruct resource = list[id];
	ResourceLoader manager(resource.id, resource.type);
	return manager.getResourceString();
}
