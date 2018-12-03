#pragma once

namespace Log {
	enum class Level : char {
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3
	};
	void setColor(int color);
	void debug(const char* format, ...);
	void info(const char* format, ...);
	void warn(const char* format, ...);
	void error(const char* format, ...);
	void fatal(const char* format, ...);
	void setLogLevel(Log::Level logLevel);
	Level getLogLevel();
};