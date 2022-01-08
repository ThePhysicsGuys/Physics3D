#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

namespace Util {

	std::string demangle(const std::string& fullName);
	std::string decamel(const std::string& string);

	template<typename T>
	std::string typeName() {
		return typeid(T).name();
	}

	std::vector<std::string> split(const std::string& string, char splitter);

	bool startsWith(const std::string& string, const std::string& prefix);
	bool endsWith(const std::string& string, const std::string& suffix);

	std::string until(const std::string& string, char end);

	std::string ltrim(const std::string& string);
	std::string rtrim(const std::string& string);
	std::string trim(const std::string& string);

	template<typename... Args>
	std::string format(const std::string& format, Args... args) {
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
		if (size_s <= 0) 
			throw std::runtime_error("Error during formatting.");
		
		auto size = static_cast<size_t>(size_s);
		auto buffer = std::make_unique<char[]>(size);
		std::snprintf(buffer.get(), size, format.c_str(), args ...);

		return std::string(buffer.get(), buffer.get() + size - 1); // We don't want the '\0' inside
	}

};