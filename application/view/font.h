#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../engine/math/vec.h"

#include <string> 

class Texture;

#define CHARACTERCOUNT 128
constexpr auto BUFFERSIZE = 30;

class Font {
private:
	Texture* atlas = nullptr;

	struct CharacterData {
		float x0;
		float y0;
		float s0;
		float t0;

		float x1;
		float y1;
		float s1;
		float t1;

		float x2;
		float y2;
		float s2;
		float t2;

		float x3;
		float y3;
		float s3;
		float t3;
	};

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
	
	unsigned int vao;
	unsigned int vbo;
	int bufferIndex = 0;
	CharacterData characterBuffer[BUFFERSIZE];

	void initFontBuffers();
	void initFontAtlas(std::string font);
	void bufferCharacter(int index, double x, double y, double size);
	void renderBuffer(int count);

public:
	Font(std::string font);

	Vec2 size(const std::string& text, double scale);

	void render(const std::string& text, Vec2 position, Vec3 color, double scale);
	void render(const std::string& text, Vec2 position, Vec4 color, double scale);
	void render(const std::string& text, double x, double y, Vec4 color, double scale);
};