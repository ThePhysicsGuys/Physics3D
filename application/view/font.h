#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h"
#include "../engine/math/vec4.h"

#include <string> 

class Texture;

#define GLYPHCOUNT 128

class Font {
private:
	unsigned int vao;
	unsigned int vbo;

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

	} characters[GLYPHCOUNT];
	
	void initFontBuffers();
	void initFontAtlas(std::string font);

	Texture* atlas = nullptr;
public:
	Font(std::string font);

	Vec2 size(const std::string& text, double scale);

	void render(const std::string& text, Vec2 position, Vec3 color, double scale);
	void render(const std::string& text, Vec2 position, Vec4 color, double scale);
	void render(const std::string& text, double x, double y, Vec4 color, double scale);
};