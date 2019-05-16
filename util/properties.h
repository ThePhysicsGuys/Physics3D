#pragma once

#include <string>
#include <map>	

#include "log.h"

class Properties {
private:
	std::map<std::string, std::string> properties;
public:
	Properties() {};

	std::string get(std::string property);
	std::map<std::string, std::string> get();
	void add(const std::string& property, const std::string& value);
	void remove(const std::string& property);

};

namespace PropertiesParser {
	Properties read(const std::string& file);
	void write(const std::string& file, Properties& properties);
};