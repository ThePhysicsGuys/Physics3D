#pragma once

#include "../engine/math/vec.h"

#include <string> 

#include <ft2build.h>
#include FT_FREETYPE_H

class Texture;

#define CHARACTERCOUNT 128

class Font {
public:
	Texture* atlas = nullptr;

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

		Character() {};
		Character(int x, int y, int width, int height, int bx, int by, int advance) : x(x), y(y), width(width), height(height), bx(bx), by(by), advance(advance) {};

	} characters[CHARACTERCOUNT];

	Font(std::string font);

	~Font();
	Font(Font&& other);
	Font(const Font&) = delete;
	Font& operator=(Font&& other);
	Font& operator=(const Font&) = delete;

	void close();

	Vec2 size(const std::string& text, double scale);

	unsigned int getAtlasID() const;
	unsigned int getAtlasWidth() const;
	unsigned int getAtlasHeight() const;
};