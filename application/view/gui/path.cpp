#include "path.h"

#include <vector>

#include "gui.h"
#include "../screen.h"
#include "../engine/math/mathUtil.h"

namespace Path {


	// Batch

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch. 
	//? Expects vertices in clockwise order
	void pushQuad(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& colorA = Vec4f(1), const Vec4f& colorB = Vec4f(1), const Vec4f& colorC = Vec4f(1), const Vec4f& colorD = Vec4f(1), const Vec2f& uvA = Vec2f(1), const Vec2f& uvB = Vec2f(1), const Vec2f& uvC = Vec2f(1), const Vec2f& uvD = Vec2f(1)) {
		GUI::batch->pushVertex({ a, uvA, colorA });
		GUI::batch->pushVertex({ b, uvB, colorB });
		GUI::batch->pushVertex({ c, uvC, colorC });
		GUI::batch->pushVertex({ d, uvD, colorD });

		GUI::batch->pushIndex(0);
		GUI::batch->pushIndex(1);
		GUI::batch->pushIndex(2);
		GUI::batch->pushIndex(2);
		GUI::batch->pushIndex(3);
		GUI::batch->pushIndex(0);

		GUI::batch->endIndex();
	}

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch
	//? Expects vertices in clockwise order
	void pushTriangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& colorA = Vec4f(1), const Vec4f& colorB = Vec4f(1), const Vec4f& colorC = Vec4f(1), const Vec2f& uvA = Vec2f(1), const Vec2f& uvB = Vec2f(1), const Vec2f& uvC = Vec2f(1)) {
		GUI::batch->pushVertex({ a, uvA, colorA });
		GUI::batch->pushVertex({ b, uvB, colorB });
		GUI::batch->pushVertex({ c, uvC, colorC });

		GUI::batch->pushIndex(0);
		GUI::batch->pushIndex(1);
		GUI::batch->pushIndex(2);

		GUI::batch->endIndex();
	}

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch
	void pushLine(const Vec2f& a, const Vec2f& b, const Vec4f& colorA, const Vec4f& colorB, float thickness) {
		Vec2f dxy = normalize(Vec2f(b.y - a.y, a.x - b.x)) / GUI::screen->dimension.y * 1.5 * thickness;

		pushQuad(Vec2f(a + dxy), Vec2f(b + dxy), Vec2f(b - dxy), Vec2f(a - dxy), colorA, colorB, colorB, colorA);
	}

	
	// Primitives

	void line(const Vec2f& a, const Vec2f& b, float thickness, const Vec4f& colorA, const Vec4f& colorB) {
		int vertexCount = 4;
		int indexCount = 6;
		GUI::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, colorA, colorB, thickness);
	}

	void circle(const Vec2f& center, float radius, const Vec4f& color, float thickness, int precision) {
		int vertexCount = 4 * precision;
		int indexCount = 6 * precision;
		GUI::batch->reserve(vertexCount, indexCount);

		Vec2f oldPoint = center + Vec2f(radius, 0);
		float step = 2.0 * PI / (float) precision;
		for (int i = 1; i <= precision; i++) {
			float angle = i * step;

			Vec2f newPoint = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));

			pushLine(oldPoint, newPoint, color, color, thickness);

			oldPoint = newPoint;
		}
	}

	void circleFilled(const Vec2f& center, float radius, const Vec4f& color, int precision) {
		int vertexCount = precision;
		int indexCount = 3 * (precision - 2);
		GUI::batch->reserve(vertexCount, indexCount);
				
		float step = 2.0f * PI / (float) precision;
		for (int i = 0; i < precision; i++) {
			float angle = i * step;

			Vec2f point = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));
			GUI::batch->pushVertex({ point, Vec2(1), color });
		}

		for (int i = 1; i < precision - 1; i++) {
			GUI::batch->pushIndex(0);
			GUI::batch->pushIndex(i + 1);
			GUI::batch->pushIndex(i);
		}

		GUI::batch->endIndex();
	}

	void circleSegment(const Vec2f& center, float radius, float minAngle, float maxAngle, bool sides, const Vec4f& color, float thickness, int precision) {
		int vertexCount = 4 * (precision + (sides? 2 : 0));
		int indexCount = 6 * (precision + (sides? 2 : 0));
		GUI::batch->reserve(vertexCount, indexCount);
		
		Vec2f oldPoint = Vec2f(center.x + radius * cos(minAngle), center.y + radius * sin(minAngle));
		
		if (sides)
			pushLine(center, oldPoint, color, color, thickness);

		float step = (maxAngle - minAngle) / (float)precision;
		for (int i = 1; i <= precision; i++) {
			float angle = minAngle + i * step;

			Vec2f newPoint = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));

			pushLine(oldPoint, newPoint, color, color, thickness);

			oldPoint = newPoint;
		}

		if (sides)
			pushLine(center, oldPoint, color, color, thickness);
	}

	void circleSegmentFilled(const Vec2f& center, float radius, float minAngle, float maxAngle, const Vec4f& color, int precision) {
		int vertexCount = precision + 2;
		int indexCount = 3 * precision;
		GUI::batch->reserve(vertexCount, indexCount);
		
		Vec2f oldPoint = Vec2f(center.x + radius * cos(minAngle), center.y + radius * sin(minAngle));
		
		GUI::batch->pushVertex({ center, Vec2f(1.0f), color });
		GUI::batch->pushVertex({ oldPoint, Vec2f(1.0f), color });

		float step = (maxAngle - minAngle) / (float) precision;
		for (int i = 1; i <= precision; i++) {
			float angle = minAngle + i * step;

			Vec2f newPoint = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));

			GUI::batch->pushVertex({ newPoint, Vec2(1), color });
		}

		for (int i = 0; i < precision + 1; i++) {
			GUI::batch->pushIndex(0);
			GUI::batch->pushIndex(i + 1);
			GUI::batch->pushIndex(i);
		}

		GUI::batch->endIndex();
	}

	void triangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& color, float thickness) {
		int vertexCount = 4 * 3;
		int indexCount = 6 * 3;
		GUI::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, color, color, thickness);
		pushLine(b, c, color, color, thickness);
		pushLine(c, a, color, color, thickness);
	}

	void triangleFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec4f& color) {
		int vertexCount = 3;
		int indexCount = 3;
		GUI::batch->reserve(vertexCount, indexCount);

		pushTriangle(a, b, c, color, color, color);
	}

	void rect(const Vec2f& pos, const Vec2f& dim, float rounding, const Vec4f& color, float thickness) {
		if (rounding == 0.0) {
			quad(pos, pos + Vec2f(dim.x, 0), pos + dim, pos + Vec2f(0, dim.y), color, thickness);
		}
		// TODO add rounding
	}

	void rectFilled(const Vec2f& pos, const Vec2f& dim, float rounding, const Vec4f& color) {
		if (rounding == 0) {
			int vertexCount = 4;
			int indexCount = 6;
			GUI::batch->reserve(vertexCount, indexCount);

			pushQuad(pos, pos + Vec2f(dim.x, 0), pos + dim, pos + Vec2f(0, dim.y), color, color, color, color);
		}
		// TODO add rounding
	}

	void quad(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color, float thickness) {
		int vertexCount = 4 * 4;
		int indexCount = 6 * 4;
		GUI::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, color, color, thickness);
		pushLine(b, c, color, color, thickness);
		pushLine(c, d, color, color, thickness);
		pushLine(d, a, color, color, thickness);
	}

	void quadFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec4f& color) {
		int vertexCount = 4;
		int indexCount = 6;
		GUI::batch->reserve(vertexCount, indexCount);

		pushQuad(a, b, c, d, color, color, color, color);
	}

	void bezier(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, float thickness, const Vec4f& color, int precision) {
		int vertexCount = 4 * precision;
		int indexCount = 6 * precision;
		GUI::batch->reserve(vertexCount, indexCount);

		Vec2f oldPoint = a;
		float step = 1.0f / (float)precision;
		for (int i = 1; i <= precision; i++) {
			float t = i * step;
			float s = 1 - t;

			float w1 = s * s * s;
			float w2 = 3.0f * s * s * t;
			float w3 = 3.0f * s * t * t;
			float w4 = t * t * t;

			Vec2f newPoint = w1 * a + w2 * b + w3 * c + w4 * d;
			pushLine(oldPoint, newPoint, color, color, thickness);

			oldPoint = newPoint;
		}
	}

	void polyLine(Vec2f* points, size_t size, float thickness, const Vec4f& color, bool closed) {
		if (size == 0)
			return; 

		if (size == 1)
			return; // Points not supported yet

		int vertexCount = 4 * (size - (closed? 0 : 1));
		int indexCount = 6 * (size - (closed ? 0 : 1));

		GUI::batch->reserve(vertexCount, indexCount);

		for (int i = 0; i < size - 1; i++)
			pushLine(points[i], points[i + 1], color, color, thickness);
		
		if (closed)
			pushLine(points[size - 1], points[0], color, color, thickness);
	}

	void polygonFilled(Vec2f* points, size_t size, const Vec4f & color) {
		if (size == 0) {
			return;
		}

		if (size == 1) {
			// point not supported
			return;
		}

		if (size == 2) {
			line(points[0], points[1], 1.0f, color, color);
			return;
		}

		int vertexCount = size;
		int indexCount = 3 * (size - 2);

		GUI::batch->reserve(vertexCount, indexCount);

		for (int i = 0; i < size; i++)
			GUI::batch->pushVertex({ points[i], Vec2f(1), color });

		for (int i = 0; i < size - 2; i++) {
			GUI::batch->pushIndex(0);
			GUI::batch->pushIndex(i + 1);
			GUI::batch->pushIndex(i + 2);
		}

		GUI::batch->endIndex();
	}

	void catmullRom(Vec2f* points, size_t size, int precision, float thickness, const Vec4f& color, bool closed, float tension, float alpha) {
		// Checks
		if (tension == 1.0f)
			polyLine(points, size, thickness, color, closed);

		if (size == 0)
			return;

		if (size == 1)
			return; // Points not supported yet

		if (size == 2)
			line(points[0], points[1], thickness, color, color);


		// Build lists
		Vec2f* pList = (Vec2f*) alloca((size + 1) * sizeof(Vec2f));
		float* tList = (float*) alloca((size + 1) * sizeof(float));

		Vec2 start = points[0] * 2 - points[1];
		Vec2 end = points[size - 1] * 2 - points[size - 2];
		float alpha2 = alpha / 2.0f;

		pList[0] = points[0] - start;
		tList[0] = pow(lengthSquared(pList[0]), alpha2);
		for (int i = 1; i < size; i++) {
			pList[i] = points[i] - points[i - 1];
			tList[i] = pow(lengthSquared(pList[i]), alpha2);
		}
		pList[size] = end - points[size - 1];
		tList[size] = pow(lengthSquared(pList[size]), alpha2);


		// Reserve vertices
		int vertexCount = 4 * size * precision;
		int indexCount = 6 * size * precision;

		GUI::batch->reserve(vertexCount, indexCount);

		float s = 1.0f - tension;
		float step = 1.0f / (float)precision;

		for (int i = 1; i < size; i++) {
			// Calculate segment
			float t012 = tList[i - 1] + tList[i];
			Vec2f p20 = pList[i - 1] + pList[i];

			float t123 = tList[i] + tList[i + 1];
			Vec2f p31 = pList[i] + pList[i + 1];

			Vec2f m1 = s * (pList[i] + tList[i] * (pList[i - 1] /  tList[i - 1] - p20 / t012));
			Vec2f m2 = s * (pList[i] + tList[i] * (pList[i + 1] / tList[i + 1] - p31 / t123));

			Vec2f a = -2.0f * pList[i] + m1 + m2;
			Vec2f b = 3.0f * pList[i] - m1 - m1 - m2;
			Vec2f c = m1;
			Vec2f d = points[i - 1];
			
			// Split segment
			Vec2f oldPoint = d;
			for (int j = 1; j <= precision; j++) {
				float t = j * step;
				float w4 = 1.0f;
				float w3 = w4 * t;
				float w2 = w3 * t;
				float w1 = w2 * t;

				Vec2f newPoint = a * w1 + b * w2 + c * w3 + d * w4;

				pushLine(oldPoint, newPoint, color, color, thickness);

				oldPoint = newPoint;
			}
		}
	}

	// Path

	std::vector<Vec2f> path;

	void lineTo(const Vec2f& vertex) {
		path.push_back(vertex);
	}

	void arcTo(const Vec2f& center, float radius, float minAngle, float maxAngle, int precision) {
		if (precision == 0 || radius == 0.0f) {
			path.push_back(center);
			return;
		}

		path.reserve(path.size() + precision + 1);

		for (int i = 0; i <= precision; i++) {
			float angle = minAngle + ((float)i / (float)precision) * (maxAngle - minAngle);
			path.push_back(Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle)));
		}
	}

	void bezierTo(const Vec2f& end, const Vec2f& tc1, const Vec2f& tc2, int precision) {
		Vec2f start = path.back();

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

	void bezierTo(const Vec2f& end, int precision) {
		Vec2f start = path.back();

		float mid = (start.y + end.y) / 2.0f;
		Vec2f c1 = Vec2f(start.x, mid);
		Vec2f c2 = Vec2f(end.x, mid);

		bezierTo(end, c1, c2, precision);
	}

	void clear() {
		path.clear();
	}

	void stroke(Vec4f color, float thickness, bool closed) {
		polyLine(path.data(), path.size(), thickness, color, closed);

		clear();
	}

	void fill(Vec4f color) {
		polygonFilled(path.data(), path.size(), color);

		clear();
	}
}