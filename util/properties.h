#pragma once

#include <string>
#include <map>	
#include <regex>

namespace Util {

class Properties {
private:	
	std::string filename;
	std::map<std::string, std::string> properties;

public:
	static std::regex regex;
	
	inline Properties() {};
	inline Properties(const std::string& filename) : filename(filename) {};

	std::string get(const std::string& property) const;
	std::map<std::string, std::string> get();
	void set(const std::string& property, const std::string& value);
	void remove(const std::string& property);

};

namespace PropertiesParser {
	Properties read(const std::string& file);
	void write(const std::string& file, Properties& properties);
};

};