#pragma once

#include "../engine/math/vec.h"
#include "../gui/gui.h"

namespace Path {
	
	// Primitives

	// Adds a line to the batch
	void line(const Vec2f& a, const Vec2f& b, float thickness = 1.0f, const Vec4f& colorA = GUI::COLOR::WHITE, const Vec4f& colorB = GUI::COLOR::WHITE);

	// Adds a circle tott the batch
	void circle(const Vec2f& center, float radius, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, int precision = 20);

	// Adds a filled circle to the batch
	void circleFilled(const Vec2f& center, float radius, const Vec4f& color = GUI::COLOR::WHITE, int precision = 20);

	// Adds a circle tott the batch
	void circleSegment(const Vec2f& center, float radius, float minAngle, float maxAngle, bool sides, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, int precision = 20);

	// Adds a filled circle to the batch
	void circleSegmentFilled(const Vec2f& center, float radius, float minAngle, float maxAngle, const Vec4f& color = GUI::COLOR::WHITE, int precision = 20);

	// Adds a triangle to the batch
	void triangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a filled triangle to the batch
	void triangleFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds a rectangle to the batch, with pos being the topleft corner and dim the dimension
	void rect(const Vec2f& pos, const Vec2f& dim, float rounding = 0.0f, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a filled rectangle to the batch
	void rectFilled(const Vec2f& pos, const Vec2f& dim, float rounding = 0.0f, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds a quad to the batch
	void quad(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a filled quad to the batch
	void quadFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds a bezier curve to the batch, with the given control points
	void bezier(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, float thickness = 1.0f, const Vec4f& color = GUI::COLOR::WHITE, int precision = 20);

	// Adds a polyline to the batch, through the given points
	void polyLine(Vec2f* points, size_t size, float thickness = 1.0f, const Vec4f& color = GUI::COLOR::WHITE, bool closed = false);

	// Adds a polygon to the batch, with the given points
	void polygonFilled(Vec2f* points, size_t size, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds an Catmull-Rom spline to batch, interpolating the given control points
	void catmullRom(Vec2f* points, size_t size, int precision =  20, float thickness = 1.0f, const Vec4f& color = GUI::COLOR::WHITE, bool closed = false, float tension = 0.0f, float alpha = 0.5f);
	

	// Polygon building

	// Adds a vertex to the current path
	void lineTo(const Vec2f& vertex);

	// Adds and arc around the given center to the path
	void arcTo(const Vec2f& center, float radius, float minAngle, float maxAngle, int precision);

	// Adds a bezier curve to the path with the given tangent control points, starting from the last point in the path
	void bezierTo(const Vec2f& end, const Vec2f& tc1, const Vec2f& tc2, int precision);

	// Adds a bezier curve to the path, starting from the last point in the path
	void bezierTo(const Vec2f& end, int precision);

	// Removes all vertices from the path
	void clear();

	// Fills the convex polygon defined by the current path
	void fill(Vec4f color);

	// Draws the current path
	void stroke(Vec4f color, float thickness = 1.0f, bool closed = false);
}

