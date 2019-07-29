#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h"
#include "../engine/math/vec4.h"

#include <string> 
#include <map>

class Texture;

class Font {
private:
	unsigned int vao;
	unsigned int vbo;

	struct Character {
		Texture* texture;
		Vec2 size;
		Vec2 bearing;
		unsigned int advance;
	};

	std::map<char, Character> characters;

public:
	Font(std::string font);

	Vec2 size(const std::string& text, double scale);

	void render(const std::string& text, Vec2 position, Vec3 color, double scale);
	void render(const std::string& text, Vec2 position, Vec4 color, double scale);
	void render(const std::string& text, double x, double y, Vec4 color, double scale);
};