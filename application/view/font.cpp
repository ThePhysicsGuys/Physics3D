#include "font.h"

#include <map>

#include "texture.h"
#include "shader.h"

#include "../engine/math/vec2.h"

Font::Font(FontShader& shader, std::string font) : shader(shader) {
	FT_Library  library;
	FT_Face face;
	FT_Error error;

	Log::setSubject(font);
	error = FT_Init_FreeType(&library);
	if (error) 
		Log::error("Error loading freetype library");

	error = FT_New_Face(library, font.c_str(), 0, &face);

	if (error == FT_Err_Unknown_File_Format) 
		Log::error("FREETYTPE: Font format not supported");
	else if (error)
		Log::error("FREETYTPE: Failed to read font");
	
	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			Log::error("FREETYTPE: Failed to load Glyph");
			continue;
		}

		Texture* texture = new Texture(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer, GL_RED);

		Character character = {
			texture,
			Vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		characters.insert(std::pair<char, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	Log::resetSubject();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Vec2 Font::size(const std::string& text, double size) {
	std::string::const_iterator iterator;
	double width = 0;
	double height = 0;
	//double ymax = 0;
	//double ymin = 0;
	for (iterator = text.begin(); iterator != text.end(); iterator++) {
		Character character = characters[*iterator];
		double advance = character.advance >> 6;
		if (iterator == text.begin())
			width += (advance - character.bearing.x) * size;
		else if (iterator == text.end() - 1)
			width += (character.bearing.x + character.size.x) * size;
 		else 
			width += advance * size;
		
		height = fmax(character.size.y * size, height);
		//ymax = fmax(character.bearing.y * size, ymax);
		//ymin = fmin((character.bearing.y - character.size.y) * size, ymin);
	}
	
	return Vec2(width, height);
}

void Font::render(const std::string& text, Vec2 position, Vec3 color, double size) {
	render(text, position.x, position.y, Vec4(color.x, color.y, color.z, 1), size);
}

void Font::render(const std::string& text, Vec2 position, Vec4 color, double size) {
	render(text, position.x, position.y, color, size);
}

void Font::render(const std::string& text, double x, double y, Vec4 color, double size) {
	shader.updateColor(color);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(vao);

	std::string::const_iterator iterator;
	for (iterator = text.begin(); iterator != text.end(); iterator++) {
		Character character = characters[*iterator];
		float descend = character.size.y - character.bearing.y;
		float xpos = x + character.bearing.x * size;
		float ypos = y - descend * size;

		float w = character.size.x * size;
		float h = character.size.y * size;
		
		float vertices[6][4] = {
			{ xpos,     ypos + h,	0, 0 },
			{ xpos,     ypos    ,	0, 1 },
			{ xpos + w, ypos    ,	1, 1 },

			{ xpos,     ypos + h,	0, 0 },
			{ xpos + w, ypos    ,   1, 1 },
			{ xpos + w, ypos + h,   1, 0 }
		};

		character.texture->bind();
		shader.updateTexture(character.texture);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (character.advance >> 6) * size; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
