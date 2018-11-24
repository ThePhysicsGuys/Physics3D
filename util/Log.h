#pragma once

namespace Log {
	enum Level : char {
		LEVEL_INFO = 0,
		LEVEL_WARNING = 1,
		LEVEL_ERROR = 2,
		LEVEL_FATAL = 3
	};
	void init();
	void debug(const char* format, ...);
	void info(const char* format, ...);
	void warn(const char* format, ...);
	void error(const char* format, ...);
	void fatal(const char* format, ...);
	void setLogLevel(Level logLevel);
	Level getLogLevel();
};