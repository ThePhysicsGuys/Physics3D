#include "log.h"

#include <stack>
#include <iostream>
#include <stdarg.h>

#include "terminalColor.h"

namespace Log {
	Level logLevel = Level::INFO;

	std::stack<std::string> subjects;
	std::string lastSubject = "";
	bool newSubject = false;

	std::string delimiter = "\n";

	std::string topSubject() {
		if (subjects.empty())
			return "";

		return subjects.top();
	}

	subject::subject(const std::string& title) {
		if (subjects.empty()) {
			subjects.push(title);
			newSubject = title != lastSubject;
		} else {
			if (title == subjects.top()) {
				newSubject = false;
			} else if (title == lastSubject) {
				newSubject = false;
			} else {
				newSubject = true;
			}
			subjects.push(title);
		}
	}

	subject::~subject() {
		lastSubject = subjects.top();
		subjects.pop();
		newSubject = lastSubject != topSubject();
	}

	bool emptySubject() {
		return topSubject().empty();
	}

	void setLogLevel(Level level) {
		logLevel = level;
	}

	Level getLogLevel() {
		return logLevel;
	}

	void printSubject(std::string subject) {
		setColor(TerminalColor::MAGENTA);
		std::cout << subject;
		setColor(TerminalColor::WHITE);
		newSubject = false;
	}

	void setDelimiter(std::string delimiter) {
		Log::delimiter = delimiter;
	}

	void debug(std::string format, ...) {
		if (logLevel != Level::NONE) {
			if (newSubject && !topSubject().empty()) printSubject("\n[" + topSubject() + "]:\n");
			if (!emptySubject()) printSubject("|\t");
			std::string message = "[DEBUG]: " + format + delimiter;
			setColor(TerminalColor::GREEN);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor(TerminalColor::WHITE);
		}
	}

	void info(std::string format, ...) {
		if (logLevel <= Level::INFO) {
			if (newSubject && !topSubject().empty()) printSubject("\n[" + topSubject() + "]:\n");
			if (!emptySubject()) printSubject("|\t");
			std::string message = "[INFO]: " + format + delimiter;
			setColor(TerminalColor::AQUA);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor(TerminalColor::WHITE);
		}
	}

	void warn(std::string format, ...) {
		if (logLevel <= Level::WARNING) {
			if (newSubject && !topSubject().empty()) printSubject("\n[" + topSubject() + "]:\n");
			if (!emptySubject()) printSubject("|\t");
			std::string message = "[WARN]: " + format + delimiter;
			setColor(TerminalColor::YELLOW);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor(TerminalColor::WHITE);
		}
	}

	void error(std::string format, ...) {
		if (logLevel <= Level::ERROR) {
			if (newSubject && !topSubject().empty()) subject("\n[" + topSubject() + "]:\n");
			if (!emptySubject()) subject("|\t");
			std::string message = "[ERROR]: " + format + delimiter;
			setColor(TerminalColor::RED);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor(TerminalColor::WHITE);
		}
	}

	void fatal(std::string format, ...) {
		if (logLevel <= Level::FATAL) {
			if (newSubject && !topSubject().empty()) printSubject("\n[" + topSubject() + "]:\n");
			if (!emptySubject()) printSubject("|\t");
			std::string message = "[FATAL]: " + format + delimiter;
			setColor(TerminalColor::BLACK, TerminalColor::RED);
			va_list args;
			va_start(args, format);
			vprintf(message.c_str(), args);
			va_end(args);
			setColor(TerminalColor::WHITE);
		}
	}

	void print(std::string format,  ...) {
		setColor(TerminalColor::WHITE);
		va_list args;
		va_start(args, format);
		vprintf(format.c_str(), args);
		va_end(args);
	}
}
