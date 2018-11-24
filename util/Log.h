#pragma once

namespace Log {
	enum Level : char {
		LEVEL_INFO = 0,
		LEVEL_WARNING = 1,
		LEVEL_ERROR = 2,
		LEVEL_FATAL = 3
	};
	void init();
	void debug(const char* msg);
	void info(const char* msg);
	void warn(const char* msg);
	void error(const char* msg);
	void fatal(const char* msg);
	void setLogLevel(Level logLevel);
	Level getLogLevel();
};