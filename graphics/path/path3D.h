#pragma once

#include "../batch/commandBatch.h"

namespace Path3D {

	//! Batch
	
	struct Vertex {
		Vec3f pos;
		Vec4f col;
	};

	extern Batch<Vertex>* batch;

	void bind(Batch<Vertex>* batch);


	//! Primitives

	void line(const Vec3f& a, const Vec3f& b, float thickness = 1.0f, const Vec4f& colorA = Vec4f(1.0f), const Vec4f& colorB = Vec4f(1.0f));

	void circle(const Vec3f& center, float radius, const Vec3f& normal, float thickness = 1.0f, const Vec4f& color = Vec4f(1.0f), size_t precision = 20);

	void polyLine(Vec3f* points, size_t size, float thickness = 1.0f, const Vec4f& color = Vec4f(1.0f), bool closed = false);


	//! Polygon building

	// Adds a vertex to the current path
	void lineTo(const Vec3f& vertex);

	// Adds a bezier curve to the path with the given tangent control points, starting from the last point in the path
	void bezierTo(const Vec3f& end, const Vec3f& tc1, const Vec3f& tc2, size_t precision);

	// Adds a bezier curve to the path, starting from the last point in the path
	void bezierTo(const Vec3f& end, size_t precision);

	// Removes all vertices from the path
	void clear();

	// Draws the current path
	void stroke(Vec4f color, float thickness = 1.0f, bool closed = false);
}