#pragma once

#include "component.h"

class Font;

class Label : public Component {
public:
	std::string text;
	Color backgroundColor;
	Color foregroundColor;
	double scale;
	Font* font;

	Label(std::string text, double x, double y);
	Label(std::string text, double x, double y, double scale);
	Label(std::string text, double x, double y, double scale, Color color);
	Label(std::string text, double x, double y, double scale, Color color, Font* font);

	Vec2 resize() override;
	void render() override;
};