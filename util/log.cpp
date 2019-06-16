#include "log.h"

#include <iostream>
#include <windows.h>
#undef ERROR

namespace Log {

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	Level logLevel = Level::INFO;

	bool newSubject = false;
	std::string lastSubject = "";
	std::string currentSubject = "";

	std::string delimiter = "\n";


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
		setColor((int) Color::SUBJECT);
		std::cout << subject;
		setColor((int) Color::NORMAL);
		newSubject = false;
	}

	void setDelimiter(std::string delimiter) {
		Log::delimiter = delimiter;
	}

	void debug(std::string format, ...) {
		if (logLevel != Level::NONE) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[DEBUG]: " + format + delimiter;
			setColor((int) Color::DEBUG);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) Color::NORMAL);
		}
	}

	void info(std::string format, ...) {
		if (logLevel <= Level::INFO) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[INFO]: " + format + delimiter;
			setColor((int) Color::INFO);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) Color::NORMAL);
		}
	}

	void warn(std::string format, ...) {
		if (logLevel <= Level::WARNING) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[WARN]: " + format + delimiter;
			setColor((int) Color::WARNING);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) Color::NORMAL);
		}
	}

	void error(std::string format, ...) {
		if (logLevel <= Level::ERROR) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[ERROR]: " + format + delimiter;
			setColor((int) Color::ERROR);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) Color::NORMAL);
		}
	}

	void fatal(std::string format, ...) {
		if (logLevel <= Level::FATAL) {
			if (newSubject) subject("\n[" + currentSubject + "]:\n");
			if (!currentSubject.empty()) subject("|\t");
			std::string message = "[FATAL]: " + format + delimiter;
			setColor((int) Color::FATAL);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor((int) Color::NORMAL);
		}
	}

	void print(std::string format,  ...) {
		setColor((int) Color::NORMAL);
		va_list args;
		va_start(args, format);
		vprintf(format.c_str(), args);
		va_end(args);
	}

	void print(std::string format, Color color, ...) {
		setColor((int) color);
		va_list args;
		va_start(args, format);
		vprintf(format.c_str(), args);
		va_end(args);
	}
}
