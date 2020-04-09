#pragma once

#include <string>

#define LOG_DEBUG(FMT, ...) Log::debug(std::string("[%s:%d] ") + FMT, __FUNCTION__, __LINE__, __VA_ARGS__);

namespace Log {

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

	void setLogLevel(Log::Level logLevel);
	Level getLogLevel();
};
