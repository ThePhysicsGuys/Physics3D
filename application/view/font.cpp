#include "font.h"

#include "../engine/math/vec.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "texture.h"
#include "shaderProgram.h"
#include "renderUtils.h"
#include "gui/gui.h"
#include "mesh/primitive.h"

#include "../util/log.h"


Font::Font(std::string font) {
	initFontAtlas(font);
	initFontBuffers();
}

void Font::initFontBuffers() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, BUFFERSIZE * sizeof(CharacterData), NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Font::initFontAtlas(std::string font) {
	FT_Library  library;
	FT_Face face;
	FT_Error error;

	Log::setSubject(font);

	// Init
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

	// Calculate atlas dimension
	int maxCharacterHeight = (face->size->metrics.height >> 6);
	int maxDimension = maxCharacterHeight * ceil(sqrt(CHARACTERCOUNT));
	int atlasDimension = 1;

	// Resize atlas to maxDimension with powers of 2
	while (atlasDimension < maxDimension)
		atlasDimension <<= 1;

	// Allocate memory
	int penX = 0;
	int penY = 0;
	char* pixels = (char*)calloc(atlasDimension * atlasDimension, 1);

	// Render glyphs to atlas
	for (unsigned char c = 0; c < CHARACTERCOUNT; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT)) {
			Log::error("FREETYTPE: Failed to load Glyph");
			continue;
		}

		FT_Bitmap* bitmap = &face->glyph->bitmap;

		if (penX + bitmap->width >= atlasDimension) {
			penX = 0;
			penY += maxCharacterHeight;
		}

		for (int row = 0; row < bitmap->rows; row++) {
			for (int col = 0; col < bitmap->width; col++) {
				int x = penX + col;
				int y = penY + row;
				pixels[x + y * atlasDimension] = bitmap->buffer[col + row * bitmap->pitch];
			}
		}

		characters[c] = Character (
			penX,
			penY,
			bitmap->width,
			bitmap->rows + 1,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		);

		penX += maxCharacterHeight;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	atlas = new Texture(atlasDimension, atlasDimension, pixels, GL_RED);

	Log::resetSubject();
}

Vec2 Font::size(const std::string& text, double size) {
	std::string::const_iterator iterator;
	double width = 0;
	double height = 0;

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
	}
	
	return Vec2(width, height);
}

void Font::bufferCharacter(int index, double x, double y, double size) {
	const Character& character = characters[index];
	float descend = character.height - character.by;
	float xpos = x + character.bx * size;
	float ypos = y - descend * size;
	
	float w = character.width * size;
	float h = character.height * size;
	
	float s = float(character.x) / atlas->width;
	float t = float(character.y) / atlas->height;
	float ds = float(character.width) / atlas->width;
	float dt = float(character.height) / atlas->height;

	characterBuffer[bufferIndex] = {
		xpos	, ypos + h, s 	  , t,
		xpos	, ypos    , s 	  , t + dt,
		xpos + w, ypos    , s + ds, t + dt,
		xpos + w, ypos + h, s + ds, t
	};
}

void Font::renderBuffer(int count) {
	// Fill GPU buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(CharacterData) * count, &characterBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Draw GPU buffer
	glDrawArrays(GL_QUADS, 0, 4 * count);
}

void Font::render(const std::string& text, double x, double y, Vec4 color, double size) {

	glBindVertexArray(vao);

	Shaders::fontShader.updateColor(color);
	Shaders::fontShader.updateTexture(atlas);
	
	for (int index = 0; index < text.size(); index++, bufferIndex++) {
		char character = text.at(index);

		if (bufferIndex == BUFFERSIZE) {
			renderBuffer(BUFFERSIZE);
			bufferIndex = 0;
		}

		if (index == text.size() - 1) {
			bufferCharacter(character, x, y, size);
			renderBuffer(bufferIndex + 1);
			bufferIndex = 0;
			break;
		}	
			   
		bufferCharacter(character, x, y, size);

		x += (characters[character].advance >> 6) * size;
	}

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::render(const std::string& text, Vec2 position, Vec3 color, double size) {
	render(text, position.x, position.y, Vec4(color.x, color.y, color.z, 1), size);
}

void Font::render(const std::string& text, Vec2 position, Vec4 color, double size) {
	render(text, position.x, position.y, color, size);
}