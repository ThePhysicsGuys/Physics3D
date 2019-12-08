#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "texture.h"
#include <string>

#define CHARACTER_COUNT 128

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

class Font {
private:
	Texture atlas;
	Character characters[CHARACTER_COUNT];

public:
	Font();
	Font(std::string font);

	~Font();
	Font(Font&& other);
	Font(const Font&) = delete;
	Font& operator=(Font&& other);
	Font& operator=(const Font&) = delete;

	void close();

	Vec2f size(const std::string& text, double scale);
	Character& getCharacter(unsigned int id);

	unsigned int getAtlasID() const;
	unsigned int getAtlasWidth() const;
	unsigned int getAtlasHeight() const;
};