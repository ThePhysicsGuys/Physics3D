#pragma once

#include "../engine/math/vec.h"

namespace GUI {
	// Clamps a value between a range
	double clamp(double value, double min, double max);

	// Maps a value from an input range to an output range
	double map(double x, double minIn, double maxIn, double minOut, double maxOut);

	// Maps a point from screen space to view space
	Vec2 map(Vec2 point);

	// Maps a point from view space to screen space
	Vec2 unmap(Vec2 point);

	// Maps a dimension from screen space to view space
	Vec2 mapDimension(Vec2 dimension);

	// Maps a dimension from view space to screen space
	Vec2 unmapDimension(Vec2 dimension);
}