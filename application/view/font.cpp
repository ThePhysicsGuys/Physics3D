#include "core.h"

#include "font.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "texture.h"
#include "shaderProgram.h"
#include "renderUtils.h"
#include "gui/gui.h"
#include "mesh/primitive.h"
#include "path/path.h"

Font::Font() {

}

Font::Font(std::string font) {
	FT_Library library;
	FT_Face face;
	FT_Error error;

	Log::subject s(font);

	// Init
	error = FT_Init_FreeType(&library);
	if (error) {
		Log::error("Error loading freetype library");

		return;
	}

	error = FT_New_Face(library, font.c_str(), 0, &face);

	if (error ) {
		Log::error("FREETYTPE: Failed to read font (%s)", font.c_str());
		if (error == FT_Err_Unknown_File_Format)
			Log::error("FREETYTPE: Font format not supported", font.c_str());

		return;
	} 

	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Calculate atlas dimension
	int maxCharacterHeight = (face->size->metrics.height >> 6);
	int maxDimension = maxCharacterHeight * ceil(sqrt(CHARACTER_COUNT));
	int atlasDimension = 1;

	// Resize atlas to maxDimension with powers of 2
	while (atlasDimension < maxDimension)
		atlasDimension <<= 1;

	// Allocate memory
	int penX = 0;
	int penY = 0;
	char* pixels = (char*)calloc(atlasDimension * atlasDimension * 4, 1);

	// Render glyphs to atlas
	for (unsigned char c = 0; c < CHARACTER_COUNT; c++) {
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
				pixels[(x + y * atlasDimension) * 4 + 0] = 0xff;
				pixels[(x + y * atlasDimension) * 4 + 1] = 0xff;
				pixels[(x + y * atlasDimension) * 4 + 2] = 0xff;
				pixels[(x + y * atlasDimension) * 4 + 3] = bitmap->buffer[col + row * bitmap->pitch];
			}
		}

		characters[c] = Character(
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

	atlas = Texture(atlasDimension, atlasDimension, pixels, GL_RGBA);
}

Font::~Font() {
	close();
}

Font::Font(Font&& other) {
	atlas = std::move(other.atlas);

	for (int i = 0; i < CHARACTER_COUNT; i++) {
		characters[i] = other.characters[i];
	}
}

Font& Font::operator=(Font&& other) {
	if (this != &other) {
		std::swap(atlas, other.atlas);

		for (int i = 0; i < CHARACTER_COUNT; i++) {
			characters[i] = other.characters[i];
		}
	}

	return *this;
}

void Font::close() {
	atlas.close();
}

Vec2f Font::size(const std::string& text, double size) {
	std::string::const_iterator iterator;
	float width = 0.0f;
	float height = 0.0f;

	for (iterator = text.begin(); iterator != text.end(); iterator++) {
		Character& character = characters[*iterator];
		float advance = character.advance >> 6;
		if (iterator == text.begin())
			width += (advance - character.bearing.x) * size;
		else if (iterator == text.end() - 1)
			width += (character.bearing.x + character.size.x) * size;
 		else 
			width += advance * size;
		
		height = fmax(character.size.y * size, height);
	}
	
	return Vec2f(width, height);
}

unsigned int Font::getAtlasID() const {
	return atlas.getID();
}

unsigned int Font::getAtlasWidth() const {
	return atlas.getWidth();
}

unsigned int Font::getAtlasHeight() const {
	return atlas.getHeight();
}

