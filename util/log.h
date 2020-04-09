#pragma once

#include <string>

#include "terminalColor.h"

#define LOG_DEBUG(FMT, ...) Log::debug(std::string("[%s:%d] ") + FMT, __FUNCTION__, __LINE__, __VA_ARGS__);

namespace Log {
	
	namespace Color {
		const TerminalColorPair DEBUG{TerminalColor::GREEN, TerminalColor::BLACK};
		const TerminalColorPair INFO{TerminalColor::AQUA, TerminalColor::BLACK};
		const TerminalColorPair WARNING{TerminalColor::YELLOW, TerminalColor::BLACK};
		const TerminalColorPair ERROR{TerminalColor::RED, TerminalColor::BLACK};
		const TerminalColorPair FATAL{TerminalColor::BLACK, TerminalColor::RED};
		const TerminalColorPair NORMAL{TerminalColor::WHITE, TerminalColor::BLACK};
		const TerminalColorPair SUBJECT{TerminalColor::MAGENTA, TerminalColor::BLACK};
	}

	enum class Level : char {
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3,
		NONE = 4
	};
	
	class subject {
	public:
		subject(const std::string& title);
		~subject();

		subject(subject&& other) = delete;
		subject(const subject&) = delete;
		subject& operator=(subject&& other) = delete;
		subject& operator=(const subject&) = delete;
	};

	void setDelimiter(std::string delimiter);

	void debug(std::string format, ...);
	void info(std::string format, ...);
	void warn(std::string format, ...);
	void error(std::string format, ...);
	void fatal(std::string format, ...);
	void print(std::string format, ...);
	void print(TerminalColorPair color, std::string format, ...);

	void setLogLevel(Log::Level logLevel);
	Level getLogLevel();
};
