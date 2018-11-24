#include <iostream>
#include <windows.h>
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

	void debug(const char* msg) {
		SetConsoleTextAttribute(console, COLOR_DEBUG);
		std::cout << "[DEBUG]: " << msg << std::endl;
	}

	void info(const char* msg) {
		if (logLevel <= LEVEL_INFO) {
			SetConsoleTextAttribute(console, COLOR_INFO);
			std::cout << "[INFO]: " << msg << std::endl;
		}
	}

	void warn(const char* msg) {
		if (logLevel <= LEVEL_WARNING) {
			SetConsoleTextAttribute(console, COLOR_WARNING);
			std::cout << "[WARNING]: " << msg << std::endl;
		}
	}

	void error(const char* msg) {
		if (logLevel <= LEVEL_ERROR) {
			SetConsoleTextAttribute(console, COLOR_ERROR);
			std::cout << "[ERROR]: " << msg << std::endl;
		}
	}

	void fatal(const char* msg) {
		if (logLevel <= LEVEL_FATAL) {
			SetConsoleTextAttribute(console, COLOR_FATAL);
			std::cout << "[FATAL]: " << msg << std::endl;
		}
	}
}