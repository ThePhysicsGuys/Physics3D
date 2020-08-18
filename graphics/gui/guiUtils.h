#pragma once

namespace P3D::Graphics {

namespace GUI {

// Clamps a value between a range
double clamp(double value, double min, double max);
float clamp(float value, float min, float max);

// Performs a smooth Hermite interpolation between 0 and 1 when start < x < end
double smoothstep(double start, double end, double x);
float smoothstep(float start, float end, float x);

// Maps a value from an input range to an output range
double map(double x, double minIn, double maxIn, double minOut, double maxOut);

// Maps a point from screen space to view space
Vec2 map(const Vec2& point);

// Maps a point from view space to screen space
Vec2 unmap(const Vec2& point);

// Maps a dimension from screen space to view space
Vec2 mapDimension(const Vec2& dimension);

// Maps a region to a given range
Vec4 mapRegion(const Vec4& region, const Vec2& inXDimension, const Vec2& inYDimension, const Vec2& outXDimension, const Vec2& outYDimension);

// Maps a point from the input range to the output range
Vec2 mapDimension(const Vec2& point, const Vec2& inXDimension, const Vec2& inYDimension, const Vec2& outXDimension, const Vec2& outYDimension);

// Maps a dimension from view space to screen space
Vec2 unmapDimension(const Vec2& dimension);

// Return whether the given value is between min and max
bool between(double value, double min, double max);

// Returns the min and max of two numbers
std::pair<double, double> minmax(double value1, double value2);

}

};