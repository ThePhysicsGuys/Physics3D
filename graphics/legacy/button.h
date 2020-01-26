#pragma once

#include "component.h"

class Button;
class Texture;

typedef void (*ButtonAction) (Button*);

class Button : public Component {
public:
	ButtonAction action = [] (Button*) {};

	Texture* hoverTexture = nullptr;
	Texture* idleTexture = nullptr;
	Texture* pressTexture = nullptr;

	std::string text;
	Color fontColor;
	double fontSize;

	bool textured = false;
	bool hovering = false;
	bool pressed = false;

	Color hoverColor;
	Color idleColor;
	Color pressColor;

	Color borderColor;
	double borderWidth;

	void setColor(const Color& color);

	Button(double x, double y, double width, double height, bool textured);
	Button(std::string text, double x, double y, bool textured);

	void render() override;
	Vec2 resize() override;

	void press(Vec2 point) override;
	void release(Vec2 point) override;
	void enter() override;
	void exit() override;
};

