#include "properties.h"

#include <fstream>

#include "stringUtil.h"
#include "log.h"

namespace Util {

std::regex Properties::regex = std::regex(" *.+ *: *.+ *");

std::string Properties::get(const std::string& property) const {
	try {
		std::string value = properties.at(property);
		return value;
	} catch (const std::out_of_range&) {
		return "";
	}
}

std::map<std::string, std::string> Properties::get() {
	return properties;
}

void Properties::set(const std::string& property, const std::string& value) {
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
		Log::subject s(file);

		std::ifstream inputstream;
		inputstream.open(file.c_str());

		if (!inputstream.is_open()) {
			Log::error("Properties file can't be found");
			return Properties();
		}

		Properties properties(file);

		std::string line;
		while (getline(inputstream, line)) {
			line = trim(line);
			
			if (line.empty() || line.at(0) == '#') {
				continue;
			} else if (!std::regex_match(line, Properties::regex)) {
				Log::warn("Incorrect syntax: %s", line.c_str());
			} else {
				size_t pos = line.find(':');
				std::string property = rtrim(line.substr(0, pos));
				std::string value = until(ltrim(line.substr(pos + 1, line.length())), ' ');

				properties.set(property, value);
			}
		}

		inputstream.close();

		return properties;
	}

	void write(const std::string& filename, Properties& properties) {
		Log::subject s(filename);

		Log::info("Writing %d properties", properties.get().size());

		std::ifstream ifile;
		ifile.open(filename.c_str(), std::ios::in);

		if (!ifile.is_open()) 
			Log::error("Properties file can't be found, creating new one");
		
		// Read and save
		std::string line;
		std::vector<std::string> lines;
		while (getline(ifile, line)) {
			std::string trimmedLine = trim(line);

			if (trimmedLine.empty() || trimmedLine.at(0) == '#' || !std::regex_match(trimmedLine, Properties::regex)) { 
				lines.push_back(line);
				continue;
			} else {
				size_t pos = line.find(':');
				std::string property = line.substr(0, pos);
				std::string value = line.substr(pos + 1, line.length());
				std::string trimmedProperty = rtrim(property);				
				std::string trimmedValue = until(ltrim(value), ' ');
				std::string newTrimmedValue = properties.get(trimmedProperty);

				if (trimmedValue == newTrimmedValue) {
					lines.push_back(line);
					continue;
				}

				auto position = value.find(trimmedValue);
				std::string newValue = value.replace(position, position + trimmedValue.length(), newTrimmedValue);

				std::string newLine = property + ":" + newValue;

				lines.push_back(newLine);
			}
		}
		ifile.close();

		// Write
		std::ofstream ofile;
		ofile.open(filename.c_str(), std::ios::out | std::ios::trunc);
		for (std::string& line : lines) 
			ofile << line << std::endl;
		ofile.close();
	}
}

};