#pragma once

#include "component.h"

class CheckBox;
class Texture;
class Label;

typedef void (*CheckBoxAction) (CheckBox*);

class CheckBox : public Component {
public:
	CheckBoxAction action = [] (CheckBox*) {};

	Texture* pressUncheckedTexture = nullptr;
	Texture* pressCheckedTexture = nullptr;
	Texture* hoverUncheckedTexture = nullptr;
	Texture* hoverCheckedTexture = nullptr;
	Texture* uncheckedTexture = nullptr;
	Texture* checkedTexture = nullptr;
	
	bool textured = false;
	bool hovering = false;
	bool pressed = false;
	bool checked = false;

	Color pressUncheckedColor;
	Color pressCheckedColor;
	Color hoverUncheckedColor;
	Color hoverCheckedColor;
	Color uncheckedColor;
	Color checkedColor;

	double checkOffset;
	double checkBoxLabelOffset;

	Label* label;

	CheckBox(double x, double y, bool textured);
	CheckBox(std::string text, double x, double y, bool textured);
	CheckBox(double x, double y, double width, double height, bool textured);
	CheckBox(std::string text, double x, double y, double width, double height, bool textured);

	void render() override;
	Vec2 resize() override;

	void disable() override;
	void enable() override;
	void press(Vec2 point) override;
	void release(Vec2 point) override;
	void enter() override;
	void exit() override;
};
