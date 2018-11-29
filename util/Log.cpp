#include <iostream>
#include <windows.h>
#include <string>
#undef ERROR
#include "Log.h"


namespace Log {

	void init();


	enum class LevelColor {
		DEBUG = 10,
		INFO = 11,
		WARNING = 14,
		ERROR = 12,
		FATAL = 192
	};

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	Level logLevel = Level::INFO;

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
		SetConsoleTextAttribute(console, (int) LevelColor::DEBUG);
		va_list args;
		va_start(args, format);
	    vprintf(msg.c_str(), args);
		va_end(args);
	}

	void info(const char* format, ...) {
		if (logLevel <= Level::INFO) {
			std::string msg = "[INFO]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, (int) LevelColor::INFO);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}

	void warn(const char* format, ...) {
		if (logLevel <= Level::WARNING) {
			std::string msg = "[WARNING]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, (int)LevelColor::WARNING);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}

	void error(const char* format, ...) {
		if (logLevel <= Level::ERROR) {
			std::string msg = "[ERROR]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, (int)LevelColor::ERROR);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}

	void fatal(const char* format, ...) {
		if (logLevel <= Level::FATAL) {
			std::string msg = "[FATAL]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			SetConsoleTextAttribute(console, (int) LevelColor::FATAL);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
		}
	}
}