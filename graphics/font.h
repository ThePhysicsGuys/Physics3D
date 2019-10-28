#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "texture.h"

#define CHARACTER_COUNT 128

class Font {
public:
	Texture atlas;

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

		Character() : x(0), y(0), width(0), height(0), bx(0), by(0), advance(0) {};
		Character(int x, int y, int width, int height, int bx, int by, int advance) : x(x), y(y), width(width), height(height), bx(bx), by(by), advance(advance) {};

	} characters[CHARACTER_COUNT];

	Font();
	Font(std::string font);

	~Font();
	Font(Font&& other);
	Font(const Font&) = delete;
	Font& operator=(Font&& other);
	Font& operator=(const Font&) = delete;

	void close();

	Vec2f size(const std::string& text, double scale);

	unsigned int getAtlasID() const;
	unsigned int getAtlasWidth() const;
	unsigned int getAtlasHeight() const;
};