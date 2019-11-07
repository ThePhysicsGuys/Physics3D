#pragma once

#include "component.h"
#include "container.h"

class Button;
class Label;

class Frame : public Container {
private:
	enum ResizingFlags : char {
		// Not resizing
		None = 0 << 0,

		// Resizing using top handle
		resizingN = 1 << 0,

		// Resizing using right handle
		resizingE = 1 << 1,

		// Resizing using bottom handle
		resizingS = 1 << 2,

		// Resizing using left handle
		resizingW = 1 << 3
	};

	char resizeFlags = None;

public:
	Component* anchor;

	Button* closeButton;
	Button* minimizeButton;
	double buttonOffset;

	Label* title;

	double titleBarHeight;
	Vec4 titleBarColor;

	Vec4 backgroundColor;

	bool minimized = false;

	Frame();
	Frame(double x, double y, std::string title = "");
	Frame(double x, double y, double width, double height, std::string title = "");

	Vec2 resize() override;
	void render() override;

	Component* intersect(Vec2 point) override;

	void drag(Vec2 newPoint, Vec2 oldPoint) override;
	void press(Vec2 point) override;
	void release(Vec2 point) override;
	void disable() override;
	void enable() override;
};

