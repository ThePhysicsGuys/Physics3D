#pragma once

#include "../gui/gui.h"

#define DEFAULT_SCISSOR Vec4f(0, 0, GUI::windowInfo.dimension.x, GUI::windowInfo.dimension.y);

namespace Path {

	//! Pattern

	typedef std::function<Vec4f(int, const Vec2f&)> Pattern2D;


	//! Flags

	enum TextFlags : char {
		// Align text pivot horizontal left
		TextPivotHL = 1 << 0,

		// Align text pivot horizontal centered
		TextPivotHC = 1 << 1,

		// Align text pivot horizontal right
		TextPivotHR = 1 << 2,

		// Align text pivot vertical top
		TextPivotVT = 1 << 3,

		// Align text pivot vertical centered
		TextPivotVC = 1 << 4,

		// Align text pivot vertical bottom
		TextPivotVB = 1 << 5
	};					  


	//! Batch

	// Current batch
	extern GuiBatch* batch;

	// Binds the given batch
	void bind(GuiBatch* batch);


	//! Primitives

	// Adds a line to the batch
	void line(const Vec2f& a, const Vec2f& b, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a line to the batch
	void line(const Vec2f& a, const Vec2f& b, const Vec4f& colorA = GUI::COLOR::WHITE, const Vec4f& colorB = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a circle to the the batch
	void circle(const Vec2f& center, float radius, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, size_t precision = 20);

	// Adds a filled circle to the batch
	void circleFilled(const Vec2f& center, float radius, const Vec4f& color = GUI::COLOR::WHITE, size_t precision = 20);

	// Adds a circle tott the batch
	void circleSegment(const Vec2f& center, float radius, float minAngle, float maxAngle, bool sides, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, size_t precision = 20);

	// Adds a filled circle to the batch
	void circleSegmentFilled(const Vec2f& center, float radius, float minAngle, float maxAngle, const Vec4f& color = GUI::COLOR::WHITE, size_t precision = 20);

	// Adds a triangle to the batch
	void triangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a filled triangle to the batch
	void triangleFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds a rectangle to the batch, with pos being the topleft corner and dim the dimension
	void rect(const Vec2f& pos, const Vec2f& dim, float rounding = 0.0f, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a filled rectangle to the batch, with pos being the topleft corner and dim the dimension
	void rectFilled(const Vec2f& pos, const Vec2f& dim, float rounding = 0.0f, const Vec4f& color = GUI::COLOR::WHITE);

	//? Adds a rect with UV coordinates and a texture id to the batch, with pos being the topleft corner and dim the dimension, uvMin starts default at lower left, uvMax at upper right
	void rectUV(unsigned int id, const Vec2f& pos, const Vec2f& dim, const Vec2f& uvMin = Vec2f(0, 0), const Vec2f& uvMax = Vec2f(1, 1), const Vec4f& color = GUI::COLOR::WHITE);

	//  Adds a rect with an UV range and a texture id to the batch, with pos being the topleft corner and dim the dimension
	void rectUVRange(unsigned int id, const Vec2f& pos, const Vec2f& dim, const Vec2f& xRange, const Vec2f& yRange, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds a quad to the batch
	void quad(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f);

	// Adds a filled quad to the batch
	void quadFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color = GUI::COLOR::WHITE);

	//? Adds a quad with UV coordinates and a texture id to the batch, default starting at the upper left corner and going clockwise
	void quadUV(unsigned int id, const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec2f& uvA = Vec2f(0, 0), const Vec2f& uvB = Vec2f(0, 1), const Vec2f& uvC = Vec2f(1, 1), const Vec2f& uvD = Vec2f(0, 1));

	// Adds a string to the batch with the given font
	void text(Font* font, const std::string& text, double size, const Vec2f& pos, const Vec4f& color = GUI::COLOR::WHITE, char textPivot = TextPivotHL | TextPivotVB);

	// Adds a bezier curve to the batch, with the given control points
	void bezier(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, size_t precision = 20);

	// Adds a bezier curve to the batch, with the given control points
	void bezier(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, Pattern2D pattern, float thickness = 1.0f, size_t precision = 20);

	// Adds a horizontal oriented bezier curve to the batch, with the given start and end
	void bezierHorizontal(const Vec2f& start, const Vec2f& end, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, size_t precision = 20);

	// Adds a horizontal oriented bezier curve to the batch, with the given start and end
	void bezierHorizontal(const Vec2f& start, const Vec2f& end, Pattern2D pattern, float thickness = 1.0f, size_t precision = 20);

	// Adds a vertical oriented bezier curve to the batch, with the given start and end
	void bezierVertical(const Vec2f& start, const Vec2f& end, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, size_t precision = 20);

	// Adds a vertical oriented bezier curve to the batch, with the given start and end
	void bezierVertical(const Vec2f& start, const Vec2f& end, Pattern2D pattern, float thickness = 1.0f, size_t precision = 20);

	// Adds a polyline to the batch, through the given points
	void polyLine(Vec2f* points, size_t size, const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, bool closed = false);

	// Adds a polyline to the batch, through the given points
	void polyLine(Vec2f* points, size_t size, Pattern2D pattern, float thickness = 1.0f, bool closed = false);

	// Adds a polygon to the batch, with the given points
	void polygonFilled(Vec2f* points, size_t size, const Vec4f& color = GUI::COLOR::WHITE);

	// Adds a polygon to the batch, with the given points
	void polygonFilled(Vec2f* points, size_t size, Pattern2D pattern);

	// Adds an Catmull-Rom spline to batch, interpolating the given control points
	void catmullRom(Vec2f* points, size_t size, const Vec4f& color = GUI::COLOR::WHITE, int precision = 20, float thickness = 1.0f, bool closed = false, float tension = 0.0f, float alpha = 0.5f);

	// Adds an Catmull-Rom spline to batch, interpolating the given control points
	void catmullRom(Vec2f* points, size_t size, Pattern2D pattern, int precision = 20, float thickness = 1.0f, bool closed = false, float tension = 0.0f, float alpha = 0.5f);
	

	//! Polygon building

	// Adds a vertex to the current path
	void lineTo(const Vec2f& vertex);

	// Adds and arc around the given center to the path
	void arcTo(const Vec2f& center, float radius, float minAngle, float maxAngle, size_t precision = 20);

	// Adds a bezier curve to the path with the given tangent control points, starting from the last point in the path
	void bezierTo(const Vec2f& end, const Vec2f& tc1, const Vec2f& tc2, size_t precision = 20);

	// Adds a bezier curve to the path, starting from the last point in the path
	void bezierTo(const Vec2f& end, size_t precision = 20);

	// Fills the convex polygon defined by the current path
	void fill(const Vec4f& color = GUI::COLOR::WHITE);

	// Fills the convex polygon defined by the current path
	void fill(Pattern2D pattern);

	// Draws the current path
	void stroke(const Vec4f& color = GUI::COLOR::WHITE, float thickness = 1.0f, bool closed = false);
	
	// Draws the current path
	void stroke(Pattern2D pattern, float thickness = 1.0f, bool closed = false);

	// Return the amount of vertices in the path
	int size();

	// Removes all vertices from the path
	void clear();
}

