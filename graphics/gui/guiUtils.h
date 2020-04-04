#pragma once

namespace Graphics {

namespace GUI {
// Clamps a value between a range
double clamp(double value, double min, double max);

// Performs a smooth Hermite interpolation between 0 and 1 when start < x < end
double smoothstep(double start, double end, double x);

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

// Return whether the given value is between min and max
bool between(double value, double min, double max);
}

};