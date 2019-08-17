#pragma once

#include "../engine/math/vec.h"
#include "../gui/gui.h"

namespace Path {
	
	// Primitives
	
	// Adds a line to the batch
	void line(const Vec2f& a, const Vec2f& b, float thickness = 1, const Vec4f& colorA = GUI::COLOR::WHITE, const Vec4f& colorB = GUI::COLOR::WHITE);

	// Polygon building

	// Adds a vertex to the current path
	void lineTo(const Vec2f& vertex);

	// Removes all vertices from the path
	void clear();

	// Fills the convex polygon defined by the current path
	void fill(Vec4f color);

	// Draws the current path
	void stroke(Vec4f color, float thickness = 1, bool closed = false);
}

