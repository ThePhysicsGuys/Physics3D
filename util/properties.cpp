#include "properties.h"

#include <fstream>

#include "stringUtil.h"

std::string Properties::get(std::string property) {
	auto iterator = properties.find(property);
	if (iterator != properties.end())
		return iterator->second;
	return nullptr;
}

std::map<std::string, std::string> Properties::get() {
	return properties;
}

void Properties::add(const std::string& property, const std::string& value) {
	if (properties.find(property) == properties.end())
		properties.insert(std::make_pair(property, value));
	else
		properties[property] = value;
}

void Properties::remove(const std::string& property) {
	for (auto iterator = properties.begin(); iterator != properties.end(); iterator++) {
		if (iterator->first == property) {
			properties.erase(iterator);
			return;
		}
	}
}

namespace PropertiesParser {
	Properties read(const std::string& file) {
		Properties properties;

		Log::setSubject(file);

		std::ifstream inputstream;
		inputstream.open(file.c_str());

		if (!inputstream.is_open()) {
			Log::error("Properties file can't be found");
			return properties;
		}

		std::string line;
		while (getline(inputstream, line)) {
			line = trim(line);
			if (line.empty() || line.at(0) == '#') {
				// next line
			} else {
				size_t pos = line.find("=");
				std::string property = rtrim(line.substr(0, pos));
				std::string value = ltrim(line.substr(pos + 1, line.length()));
				properties.add(property, value);
			}
		}

		inputstream.close();

		Log::resetSubject();

		return properties;
	}

	void write(const std::string& file, Properties& properties) {
		Log::setSubject(file);

		Log::info("Writing %d porperties", properties.get().size());

		std::ofstream outputstream;
		outputstream.open(file.c_str(), std::ofstream::out | std::ofstream::trunc);

		if (!outputstream.is_open()) {
			Log::error("Properties file can't be found, creating new one");
		}

		for (auto property : properties.get()) {
			outputstream << property.first << " = " << property.second << std::endl;
		}

		Log::resetSubject();

		outputstream.close();
	}
}