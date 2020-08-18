#include "core.h"

#include "guiUtils.h"

#include "gui.h"

namespace P3D::Graphics {

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

Vec2 map(const Vec2& point) {
	return Vec2(
		map(point.x, 0, GUI::windowInfo.dimension.x, -GUI::windowInfo.aspect, GUI::windowInfo.aspect),
		map(GUI::windowInfo.dimension.y - point.y, 0, GUI::windowInfo.dimension.y, -1, 1));
}

Vec4 mapRegion(const Vec4& region, const Vec2& inXDimension, const Vec2& inYDimension, const Vec2& outXDimension, const Vec2& outYDimension) {
	Vec2 inPoint1 = Vec2(region.x, region.y);
	Vec2 inPoint2 = Vec2(region.z, region.w);
	Vec2 outPoint1 = mapDimension(inPoint1, inXDimension, inYDimension, outXDimension, outYDimension);
	Vec2 outPoint2 = mapDimension(inPoint2, inXDimension, inYDimension, outXDimension, outYDimension);

	return Vec4(outPoint1.x, outPoint1.y, outPoint2.x, outPoint2.y);
}

Vec2 mapDimension(const Vec2& point, const Vec2& inXDimension, const Vec2& inYDimension, const Vec2& outXDimension, const Vec2& outYDimension) {
	return Vec2(map(point.x, inXDimension.x, inXDimension.y, outXDimension.x, outXDimension.y), map(point.y, inYDimension.x, inYDimension.y, outYDimension.x, outYDimension.y));
}

Vec2 unmap(const Vec2& point) {
	return Vec2(map(point.x, -GUI::windowInfo.aspect, GUI::windowInfo.aspect, 0, GUI::windowInfo.dimension.x), GUI::windowInfo.dimension.y - map(point.y, -1, 1, 0, GUI::windowInfo.dimension.y));
}

Vec2 mapDimension(const Vec2& dimension) {
	return Vec2(map(dimension.x, 0, GUI::windowInfo.dimension.x, 0, 2 * GUI::windowInfo.aspect), map(dimension.y, 0, GUI::windowInfo.dimension.y, 0, 2));
}

Vec2 unmapDimension(const Vec2& dimension) {
	return Vec2(map(dimension.x, 0, 2 * GUI::windowInfo.aspect, 0, GUI::windowInfo.dimension.x), map(dimension.y, 0, 2, 0, GUI::windowInfo.dimension.y));
}

bool between(double value, double min, double max) {
	return min <= value && value <= max;
}

std::pair<double, double> minmax(double value1, double value2) {
	return (value1 < value2) ? std::pair { value1, value2 } : std::pair { value2, value1 };
}

}

};