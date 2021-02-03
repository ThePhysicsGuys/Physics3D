#pragma once

#include <vector>
#include <string>

namespace Util {
class ParsedArgs {
	struct OptionalArg {
		std::string option;
		std::string value;
	};
	std::vector<std::string> inputArgs;
	std::vector<std::string> flags;
	std::vector<OptionalArg> optionals;
public:

	ParsedArgs(int argc, const char** argv) {
		for(int i = 1; i < argc; i++) {
			const char* item = argv[i];
			if(item[0] == '-') {
				if(item[1] == '-') {
					if(i + 1 < argc) {
						optionals.push_back(OptionalArg{std::string(item + 2), std::string(argv[i + 1])});
						i++;
					} else {
						throw "No associated value for the final optional arg";
					}
				} else {
					flags.emplace_back(item + 1);
				}
			} else {
				inputArgs.emplace_back(item);
			}
		}
	}

	bool hasFlag(const char* flag) const {
		for(const std::string& f : flags) {
			if(f == flag) {
				return true;
			}
		}
		return false;
	}

	std::string getOptional(const char* key) const {
		for(const OptionalArg& o : optionals) {
			if(o.option == key) {
				return o.value;
			}
		}
		return std::string();
	}

	std::size_t argCount() const { return inputArgs.size(); }
	const std::vector<std::string>& args() { return inputArgs; }

	const std::string& operator[](std::size_t index) const { return inputArgs[index]; }

	auto begin() const { return inputArgs.begin(); }
	auto end() const { return inputArgs.end(); }
};
};
