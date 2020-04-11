#include "core.h"

#include "guiUtils.h"

#include "gui.h"

namespace Graphics {

namespace GUI {
double clamp(double value, double min, double max) {
	if (value < min)
		return min;
	if (value > max)
		return max;
	return value;
}
float clamp(float value, float min, float max) {
	if(value < min)
		return min;
	if(value > max)
		return max;
	return value;
}

double smoothstep(double start, double end, double x) {
	double t = clamp((x - start) / (end - start), 0.0, 1.0);
	return t * t * (3.0 - 2.0 * t);
}

float smoothstep(float start, float end, float x) {
	float t = clamp((x - start) / (end - start), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

double map(double x, double minIn, double maxIn, double minOut, double maxOut) {
	return (x - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut;
}

Vec2 map(Vec2 point) {
	return Vec2(
		map(point.x, 0, GUI::windowInfo.dimension.x, -GUI::windowInfo.aspect, GUI::windowInfo.aspect),
		map(GUI::windowInfo.dimension.y - point.y, 0, GUI::windowInfo.dimension.y, -1, 1));
}

Vec2 unmap(Vec2 point) {
	return Vec2(map(point.x, -GUI::windowInfo.aspect, GUI::windowInfo.aspect, 0, GUI::windowInfo.dimension.x), GUI::windowInfo.dimension.y - map(point.y, -1, 1, 0, GUI::windowInfo.dimension.y));
}

Vec2 mapDimension(Vec2 dimension) {
	return Vec2(map(dimension.x, 0, GUI::windowInfo.dimension.x, 0, 2 * GUI::windowInfo.aspect), map(dimension.y, 0, GUI::windowInfo.dimension.y, 0, 2));
}

Vec2 unmapDimension(Vec2 dimension) {
	return Vec2(map(dimension.x, 0, 2 * GUI::windowInfo.aspect, 0, GUI::windowInfo.dimension.x), map(dimension.y, 0, 2, 0, GUI::windowInfo.dimension.y));
}

bool between(double value, double min, double max) {
	return min <= value && value <= max;
}
}

};