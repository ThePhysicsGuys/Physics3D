#include "log.h"

#include <iostream>
#include <windows.h>
#undef ERROR

namespace Log {

	enum class LevelColor {
		DEBUG = 10,
		INFO = 11,
		WARNING = 14,
		ERROR = 12,
		FATAL = 192,
		NORMAL = 15,
		SUBJECT = 13
	};

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	Level logLevel = Level::INFO;

	bool newSubject = false;
	std::string lastSubject = "";
	std::string currentSubject = "";

	void setSubject(std::string subject) {
		currentSubject = subject;
		newSubject = lastSubject != subject;
	}

	void resetSubject() {
		lastSubject = currentSubject;
		currentSubject = "";
	}

	void setLogLevel(Level level) {
		logLevel = level;
	}

	Level getLogLevel() {
		return logLevel;
	}
	
	void setColor(int color) {
		SetConsoleTextAttribute(console, color);
	}

	void subject(std::string subject) {
		setColor((int) LevelColor::SUBJECT);
		std::cout << subject;
		setColor((int) LevelColor::NORMAL);
		newSubject = false;
	}

	void debug(std::string format, ...) {
		if (logLevel != Level::NONE) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[DEBUG]: " + format + "\n";
			setColor((int) LevelColor::DEBUG);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) LevelColor::NORMAL);
		}
	}

	void info(std::string format, ...) {
		if (logLevel <= Level::INFO) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[INFO]: " + format + "\n";
			setColor((int) LevelColor::INFO);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) LevelColor::NORMAL);
		}
	}

	void warn(std::string format, ...) {
		if (logLevel <= Level::WARNING) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[WARN]: " + format + "\n";
			setColor((int) LevelColor::WARNING);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) LevelColor::NORMAL);
		}
	}

	void error(std::string format, ...) {
		if (logLevel <= Level::ERROR) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[ERROR]: " + format + "\n";
			setColor((int) LevelColor::ERROR);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) LevelColor::NORMAL);
		}
	}

	void fatal(std::string format, ...) {
		if (logLevel <= Level::FATAL) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[FATAL]: " + format + "\n";
			setColor((int) LevelColor::FATAL);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) LevelColor::NORMAL);
		}
	}
}
