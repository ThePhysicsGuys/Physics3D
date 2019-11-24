#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "texture.h"
#include "gui/text.h"

#define CHARACTER_COUNT 128

class Font {
public:
	Texture atlas;

	Character characters[CHARACTER_COUNT];

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