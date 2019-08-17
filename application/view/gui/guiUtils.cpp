#include "guiUtils.h"

#include "../screen.h"
#include "gui.h"

namespace GUI {
	double clamp(double value, double min, double max) {
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	double map(double x, double minIn, double maxIn, double minOut, double maxOut) {
		return (x - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut;
	}

	Vec2 map(Vec2 point) {
		return Vec2(
			map(point.x, 0, screen->dimension.x, -screen->camera.aspect, screen->camera.aspect),
			map(screen->dimension.y - point.y, 0, screen->dimension.y, -1, 1));
	}

	Vec2 unmap(Vec2 point) {
		return Vec2(map(point.x, -screen->camera.aspect, screen->camera.aspect, 0, screen->dimension.x), screen->dimension.y - map(point.y, -1, 1, 0, screen->dimension.y));
	}

	Vec2 mapDimension(Vec2 dimension) {
		return Vec2(map(dimension.x, 0, screen->dimension.x, 0, 2 * screen->camera.aspect), map(dimension.y, 0, screen->dimension.y, 0, 2));
	}

	Vec2 unmapDimension(Vec2 dimension) {
		return Vec2(map(dimension.x, 0, 2 * screen->camera.aspect, 0, screen->dimension.x), map(dimension.y, 0, 2, 0, screen->dimension.y));
	}
}