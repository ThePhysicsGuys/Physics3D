#include "systemVariables.h"

#include <unordered_map>
#include <stdexcept>

static std::unordered_map<std::string, int> variables;

int SystemVariables::get(const std::string& key) {
	auto iterator = variables.find(key);

	if (iterator == variables.end())
		throw std::runtime_error("System variable \"" + key + "\" not found");

	return iterator->second;
}

void SystemVariables::set(const std::string& key, int value) {
	variables[key] = value;
}