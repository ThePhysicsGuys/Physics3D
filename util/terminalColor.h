#pragma once

enum class TerminalColor {
	BLACK = 0,
	DARK_BLUE = 1,
	DARK_GREEN = 2,
	AQUA = 3,
	DARK_RED = 4,
	PURPLE = 5,
	DARK_YELLOW = 6,
	LIGHT_GRAY = 7,
	GRAY = 8,
	BLUE = 9,
	GREEN = 10,
	CYAN = 11,
	RED = 12,
	MAGENTA = 13,
	YELLOW = 14,
	WHITE = 15,
};

struct TerminalColorPair {
	TerminalColor foreground;
	TerminalColor background;
};

void setColor(TerminalColor foreground);
void setColor(TerminalColor foreground, TerminalColor background);
void setColor(TerminalColorPair color);
