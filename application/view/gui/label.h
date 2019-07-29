#pragma once

#include <string>

#include "component.h"

#include "../engine/math/vec3.h"
#include "../engine/math/vec2.h"

class Font;

class Label : public Component {
public:
	std::string text;
	Vec4 backgroundColor;
	Vec4 foregroundColor;
	double scale;
	Font* font;

	Label(std::string text, double x, double y);
	Label(std::string text, double x, double y, double scale);
	Label(std::string text, double x, double y, double scale, Vec4 color);
	Label(std::string text, double x, double y, double scale, Vec4 color, Font* font);

	Vec2 resize() override;
	void render() override;
};