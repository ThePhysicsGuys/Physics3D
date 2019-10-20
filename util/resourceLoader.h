#pragma once

#include <string>

struct ResourceStruct {
	const int id;
	const std::string type;
};

typedef ResourceStruct* ResourceList;

std::string getResourceAsString(ResourceList list, int id);
