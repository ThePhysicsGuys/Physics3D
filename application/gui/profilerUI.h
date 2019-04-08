#pragma once

#include <string>
#include "../../engine/profiling.h"
#include "screen.h"
#include "bindable.h"
#include "font.h"

extern const Vec3f pieColors[30];

struct PiePart {
	float weight;
	std::string value;
	Vec3f color;
	const char* label;
	PiePart() : color(), weight(0) {}
	PiePart(float weight, std::string value, Vec3f color, const char* label) : weight(weight), value(value), color(color), label(label) {}
};
struct PieChart {
	const char* title;
	Vec2f piePosition;
	float pieSize;
	std::vector<PiePart> parts;
	std::string totalValue;

	inline PieChart(const char* title, std::string totalValue, Vec2f piePosition, float pieSize) : title(title), totalValue(totalValue), piePosition(piePosition), pieSize(pieSize) {}
	void renderPie(Screen& screen) const;
	void renderText(Screen& screen, Font* font) const;
	void add(PiePart& p);
	float getTotal() const;
};
void startPieRendering(Screen& screen);
void endPieRendering(Screen& screen);
