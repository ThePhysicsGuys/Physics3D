#pragma once

#include <Windows.h>
#include <iostream>
#include <string>


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

	std::string getResourceString() const {
		std::string dst;
		if (p.ptr != nullptr)
			dst = std::string(reinterpret_cast<char*>(p.ptr), p.size_bytes);
		return dst;
	}
};