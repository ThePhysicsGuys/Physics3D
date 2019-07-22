#pragma once

#include <string>
#include <map>	

#include "log.h"

class Properties {
private:
	std::map<std::string, std::string> properties;
public:
	Properties() {};

	std::string NOT_FOUND = "";

	std::string get(std::string property) const;
	std::map<std::string, std::string> get();
	void set(const std::string& property, const std::string& value);
	void remove(const std::string& property);

};

namespace PropertiesParser {
	Properties read(const std::string& file);
	void write(const std::string& file, Properties& properties);
};