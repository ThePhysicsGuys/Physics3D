#include <iostream>
#include <windows.h>
#include <string>
#undef ERROR
#include "log.h"

namespace Log {

	enum class LevelColor {
		DEBUG = 10,
		INFO = 11,
		WARNING = 14,
		ERROR = 12,
		FATAL = 192,
		NORMAL = 15
	};

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	Level logLevel = Level::INFO;

	void setLogLevel(Level level) {
		logLevel = level;
	}

	Level getLogLevel() {
		return logLevel;
	}
	
	void setColor(int color) {
		SetConsoleTextAttribute(console, color);
	}

	void debug(const char* format, ...) {
		std::string msg = "[DEBUG]: " + std::string(format) + "\n";
		if (console == nullptr)
			std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
		setColor((int) LevelColor::DEBUG);
		va_list args;
		va_start(args, format);
	    vprintf(msg.c_str(), args);
		va_end(args);
		setColor((int)LevelColor::NORMAL);
	}

	void info(const char* format, ...) {
		if (logLevel <= Level::INFO) {
			std::string msg = "[INFO]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			setColor((int)LevelColor::INFO);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
			setColor((int)LevelColor::NORMAL);
		}
	}

	void warn(const char* format, ...) {
		if (logLevel <= Level::WARNING) {
			std::string msg = "[WARNING]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			setColor((int)LevelColor::WARNING);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
			setColor((int)LevelColor::NORMAL);
		}
	}

	void error(const char* format, ...) {
		if (logLevel <= Level::ERROR) {
			std::string msg = "[ERROR]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			setColor((int)LevelColor::ERROR);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
			setColor((int)LevelColor::NORMAL);
		}
	}

	void fatal(const char* format, ...) {
		if (logLevel <= Level::FATAL) {
			std::string msg = "[FATAL]: " + std::string(format) + "\n";
			if (console == nullptr)
				std::cout << "[WARNING]: Log not initialized, colors will be unavailable" << std::endl;
			setColor((int)LevelColor::FATAL);
			va_list args;
			va_start(args, format);
			vprintf(msg.c_str(), args);
			va_end(args);
			setColor((int)LevelColor::NORMAL);
		}
	}
}
