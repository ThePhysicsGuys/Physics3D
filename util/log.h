#pragma once

#include <string>

namespace Log {
	enum class Level : char {
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3,
		NONE = 4
	};
	void setSubject(std::string subject);
	void resetSubject();
	void setColor(int color);
	void debug(std::string format, ...);
	void info(std::string format, ...);
	void warn(std::string format, ...);
	void error(std::string format, ...);
	void fatal(std::string format, ...);
	void setLogLevel(Log::Level logLevel);
	Level getLogLevel();
};