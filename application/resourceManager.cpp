#include "resourceManager.h"

#include <windows.h>
#include "resource.h"

struct ResourceStruct {
	const int id;
	const std::string type;
};

ResourceStruct resources[]{
	{ IDR_SHADER1, "SHADER" },
	{ IDR_SHADER2, "SHADER" },
	{ IDR_SHADER3, "SHADER" },
	{ IDR_SHADER4, "SHADER" },
	{ IDR_SHADER5, "SHADER" },
	{ IDR_SHADER6, "SHADER" },
	{ IDR_SHADER7, "SHADER" },
	{ IDR_OBJ2, "OBJ" }
};

class ResourceManager {
public:
	struct Parameters {
		std::size_t size_bytes = 0;
		void* ptr = nullptr;
	};

private:
	HRSRC hResource = nullptr;
	HGLOBAL hMemory = nullptr;

	Parameters p;

public:
	ResourceManager(int resource_id, const std::string &resource_class) {
		hResource = FindResource(nullptr, MAKEINTRESOURCEA(resource_id), resource_class.c_str());
		hMemory = LoadResource(nullptr, hResource);

		p.size_bytes = SizeofResource(nullptr, hResource);
		p.ptr = LockResource(hMemory);
	}

	/*~ResourceManager() {
		UnlockResource(p.ptr);
	}*/

	std::string getResourceString() const {
		std::string dst;
		if (p.ptr != nullptr)
			dst = std::string(reinterpret_cast<char*>(p.ptr), p.size_bytes);
		return dst;
	}
};


std::string getResourceAsString(int res) {
	ResourceStruct r = resources[res];
	ResourceManager m(r.id, r.type);
	return m.getResourceString();
}
