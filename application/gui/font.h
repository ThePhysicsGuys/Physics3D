#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shaderProgram.h"

#include "../engine/math/vec3.h"
#include "../engine/math/vec4.h"
#include "../../util/log.h"

#include <string>   

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

	FontShader& shader;

public:
	Font(FontShader& shader, std::string font);

	Vec2 size(const std::string& text, double scale);

	void render(const std::string& text, Vec2 position, Vec3 color, double scale);
	void render(const std::string& text, Vec2 position, Vec4 color, double scale);
	void render(const std::string& text, double x, double y, Vec4 color, double scale);
};