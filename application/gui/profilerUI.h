#pragma once

#include "../../engine/profiling.h"
#include "screen.h"
#include "bindable.h"
#include "font.h"

extern const Vec3f pieColors[30];

struct PiePart {
	float weight;
	Vec3f color;
	const char* label;
	PiePart() : color(), weight(0) {}
	PiePart(float weight, Vec3f color, const char* label) : weight(weight), color(color), label(label) {}
};
struct PieChart {
	const char* title;
	const char* weightUnit;
	Vec2f piePosition;
	float pieSize;
	std::vector<PiePart> parts;

	inline PieChart(const char* title, const char* weightUnit, Vec2f piePosition, float pieSize) : title(title), weightUnit(weightUnit), piePosition(piePosition), pieSize(pieSize) {}
	void renderPie(Screen& screen) const;
	void renderText(Screen& screen, Font* font) const;
	void add(PiePart& p);
	float getTotal() const;
private:
	void renderValueUnit(Font * font, float value, const char * unit, Vec2 position, Vec3f color) const;
};
void startPieRendering(Screen& screen);
void endPieRendering(Screen& screen);
