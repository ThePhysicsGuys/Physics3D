#pragma once

#include "core.h"

struct Character {
	union {
		struct {
			int x;
			int y;
		};
		Vec2i origin;
	};

	union {
		struct {
			int width;
			int height;
		};
		Vec2i size;
	};

	union {
		struct {
			int bx;
			int by;
		};
		Vec2i bearing;
	};

	unsigned int advance;
	unsigned int id;

	Character();
	Character(unsigned int id, int x, int y, int width, int height, int bx, int by, unsigned int advance);
};

struct Word {
	// List of characters in this word
	std::vector<Character> characters;
	// Font size of the word
	double fontSize;
	// Width of the word
	double width;

	Word(double fontSize);

	void addCharacter(const Character& character);
};

struct Line {
	// Words of this line
	std::vector<Word> words;
	// The max width of this line
	double maxWidth;
	// The current width of this line
	double width;
	// The width of a space character
	double spaceWidth;

	Line(double maxWidth, double spaceWidth);

	bool addWord(const Word& word);
};

enum TextFlags : char {
	// Align text pivot horizontal left
	TextPivotHL = 1 << 0,

	// Align text pivot horizontal centered
	TextPivotHC = 1 << 1,

	// Align text pivot horizontal right
	TextPivotHR = 1 << 2,

	// Align text pivot vertical top
	TextPivotVT = 1 << 3,

	// Align text pivot vertical centered
	TextPivotVC = 1 << 4,

	// Align text pivot vertical bottom
	TextPivotVB = 1 << 5
};

class Font;

struct Text {
	std::string textString;

	Font* font;
	double fontSize;
	
	TextFlags textFlags;

	double spaceWidth;

	double x;
	double y;
	double maxWidth;

	Text(std::string textString, double x, double y, Font* font, double fontSize, double maxWidth, double spaceWidth, TextFlags textFlags);
};

class TextLoader {
public:
	std::vector<Line> loadText(const Text& text);
};