#pragma once

#include <exception>
#include <string>

struct EngineException : public std::exception {
	std::string reason;
	inline EngineException() : exception(), reason("") {}
	inline EngineException(const char* reason) : exception(), reason(reason) {};
	inline EngineException(std::string reason) : exception(reason.c_str()), reason(reason) {}
	
	virtual const char* what() const override {
		return reason.c_str();
	}
};
