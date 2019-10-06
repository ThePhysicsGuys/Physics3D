#pragma once

#include <string>

namespace Log {

	static const int BLACK = 0x0;
	static const int BLUE = 0x1;
	static const int GREEN = 0x2;
	static const int AQUA = 0x3;
	static const int RED = 0x4;
	static const int MAGENTA = 0x5;
	static const int YELLOW = 0x6;
	static const int WHITE = 0x7;
	static const int STRONG = 0x8;

	enum class Level : char {
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3,
		NONE = 4
	};
	
	enum class Color {
		DEBUG = STRONG | GREEN,
		INFO = STRONG | AQUA,
		WARNING = STRONG | YELLOW,
		ERROR = STRONG | RED,
		FATAL = (STRONG | RED) << 4,
		NORMAL = STRONG | WHITE,
		SUBJECT = STRONG | MAGENTA
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
	
	void setColor(int color);

	void debug(std::string format, ...);
	void info(std::string format, ...);
	void warn(std::string format, ...);
	void error(std::string format, ...);
	void fatal(std::string format, ...);
	void print(std::string format, ...);
	void print(std::string format, Color color, ...);

	void setLogLevel(Log::Level logLevel);
	Level getLogLevel();
};