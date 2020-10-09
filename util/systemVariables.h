#pragma once

#include <string>

struct SystemVariables {
public:
	static int get(const std::string& key);
	static void set(const std::string& key, int value);
};

