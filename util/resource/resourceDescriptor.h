#pragma once


#include <string>

#ifdef _MSC_VER
struct ResourceDescriptor {
	const int id;
	const std::string type;
};
#else
// ResourceDescriptors for reading from files
struct ResourceDescriptor {
	const char* fileName;
	const std::string type;
};
#endif