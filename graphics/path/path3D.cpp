#include "core.h"

#include "path3D.h"

namespace Path3D {

	//! Batch

	Batch<Vertex>* batch = nullptr;

	void bind(Batch<Vertex>* batch) {
		Path3D::batch = batch;
	}

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch
	void pushLine(const Vec3f& a, const Vec3f& b, const Vec4f& colorA, const Vec4f& colorB, float thickness) {
		batch->pushVertex({ a, colorA });
		batch->pushVertex({ b, colorB });

		batch->pushIndex(0);
		batch->pushIndex(1);

		batch->endIndex();
	}


	//! Primitives

	void line(const Vec3f& a, const Vec3f& b, float thickness, const Vec4f& colorA, const Vec4f& colorB) {
		int vertexCount = 4;
		int indexCount = 6;
		Path3D::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, colorA, colorB, thickness);
	}

	void polyLine(Vec3f* points, size_t size, float thickness, const Vec4f& color, bool closed) {
		if (size == 0)
			return;

		if (size == 1)
			return; // Points not supported yet

		int vertexCount = 4 * (size - (closed ? 0 : 1));
		int indexCount = 6 * (size - (closed ? 0 : 1));

		Path3D::batch->reserve(vertexCount, indexCount);

		for (int i = 0; i < size - 1; i++)
			pushLine(points[i], points[i + 1], color, color, thickness);

		if (closed)
			pushLine(points[size - 1], points[0], color, color, thickness);
	}

	//! Path

	std::vector<Vec3f> path;

	void lineTo(const Vec3f& vertex) {
		path.push_back(vertex);
	}

	void bezierTo(const Vec3f& end, const Vec3f& tc1, const Vec3f& tc2, int precision) {
		Vec3f start = path.back();

		float step = 1.0f / (float)precision;
		for (int i = 1; i <= precision; i++) {
			float t = i * step;
			float s = 1 - t;

			float w1 = s * s * s;
			float w2 = 3.0f * s * s * t;
			float w3 = 3.0f * s * t * t;
			float w4 = t * t * t;

			path.push_back(w1 * start + w2 * tc1 + w3 * tc2 + w4 * end);
		}
	}

	void bezierTo(const Vec3f& end, int precision) {
		Vec3f start = path.back();

		float midX = (start.y + end.y) / 2.0f;
		float midZ = (start.y + end.y) / 2.0f;
		Vec3f c1 = Vec3f(midX, start.y, midZ);
		Vec3f c2 = Vec3f(midX, end.y, midZ);

		bezierTo(end, c1, c2, precision);
	}

	void clear() {
		path.clear();
	}

	void stroke(Vec4f color, float thickness, bool closed) {
		polyLine(path.data(), path.size(), thickness, color, closed);

		clear();
	}

}