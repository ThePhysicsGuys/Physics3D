#pragma once

#include "../batch/batch.h"
#include "../gui/gui.h"

namespace P3D::Graphics {

namespace Path3D {

	//! Pattern

	typedef std::function<Color(int, const Vec3f&)> Pattern3D;


	//! Batch
	
	struct Vertex {
		Vec3f pos;
		Vec4f col;
	};

	extern Graphics::Batch<Vertex>* batch;

	void bind(Graphics::Batch<Vertex>* batch);


	//! Primitives

	/* Adds a line to the batch */
	void line(const Vec3f& a, const Vec3f& b, const Color& colorA = Colors::WHITE, const Color& colorB = Colors::WHITE, float thickness = 1.0f);

	/* Adds a triangle to the batch */
	void triangle(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Color& colorA = Colors::WHITE, const Color& colorB = Colors::WHITE, const Color& colorC = Colors::WHITE, float thickness = 1.0f);

	/* Adds a quad to the batch */
	void quad(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d, const Color& colorA = Colors::WHITE, const Color& colorB = Colors::WHITE, const Color& colorC = Colors::WHITE, Color colorD = Colors::WHITE, float thickness = 1.0f);

	/* Adds a circle perpendicular to the given normal to the batch */
	void circle(const Vec3f& center, float radius, const Vec3f& normal, float thickness = 1.0f, const Color& color = Colors::WHITE, size_t precision = 20);

	/* Adds a linestrip to the batch */
	void polyLine(Vec3f* points, size_t size, const Color& color = Colors::WHITE, float thickness = 1.0f, bool closed = false);

	/* Adds a linestrip to the batch */
	void polyLine(Vec3f* points, size_t size, Pattern3D pattern, float thickness = 1.0f, bool closed = false);


	//! Polygon building

	/* Adds a vertex to the current path */
	void lineTo(const Vec3f& vertex);

	/* Adds a bezier curve to the path with the given tangent control points, starting from the last point in the path */
	void bezierTo(const Vec3f& end, const Vec3f& tc1, const Vec3f& tc2, size_t precision);

	/* Adds a bezier curve to the path, starting from the last point in the path*/
	void bezierTo(const Vec3f& end, size_t precision);

	/* Draws the current path */
	void stroke(const Color& color = Colors::WHITE, float thickness = 1.0f, bool closed = false);

	/* Draws the current path */
	void stroke(Pattern3D pattern, float thickness = 1.0f, bool closed = false);

	/* Returns the amount of vertices in the current path */
	int size();

	/* Removes all vertices from the path */
	void clear();

}

};