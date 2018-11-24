#include <iostream>
#include <windows.h>
#include <string>
#include "Log.h"

namespace Log {

	enum LevelColor {
		COLOR_DEBUG = 10,
		COLOR_INFO = 11,
		COLOR_WARNING = 14,
		COLOR_ERROR = 12,
		COLOR_FATAL = 192
	};

	HANDLE console;
	Level logLevel;

	void init() {
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		setLogLevel(LEVEL_INFO);
		info("Initialized log");
	}

	void setLogLevel(Level level) {
		logLevel = level;
	}

	Level getLogLevel() {
		return logLevel;
	}

	void debug(const char* format, ...) {
		std::string msg = "[DEBUG]: " + std::string(format) + "\n";
		if (console == nullptr)
			std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
		SetConsoleTextAttribute(console, COLOR_DEBUG);
		va_list args;
		va_start(args, format);
	    vprintf(msg.c_str(), args);
		va_end(args);
	}

	void info(const char* format, ...) {
		if (logLevel <= LEVEL_INFO) {
			std::string msg = "[INFO]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, COLOR_INFO);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}

	void warn(const char* format, ...) {
		if (logLevel <= LEVEL_WARNING) {
			std::string msg = "[WARNING]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, COLOR_WARNING);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}

	void error(const char* format, ...) {
		if (logLevel <= LEVEL_ERROR) {
			std::string msg = "[ERROR]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, COLOR_ERROR);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}

	void fatal(const char* format, ...) {
		if (logLevel <= LEVEL_FATAL) {
			std::string msg = "[FATAL]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, COLOR_FATAL);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}
}