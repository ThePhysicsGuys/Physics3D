#include "core.h"

#include "path.h"

#include "font.h"
#include "../batch/guiBatch.h"
#include "../physics/math/mathUtil.h"

#define DEFAULT_PATTERN_2D(color) [color] (int i, const Vec2f& p) { return color; }

namespace Graphics {

namespace Path {

	#pragma region Batch

	//! Batch

	GuiBatch* batch = nullptr;

	void bind(GuiBatch* batch) {
		Path::batch = batch;
	}

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch. 
	//? Expects vertices in counter-clockwise order
	void pushQuad(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Color& colorA = Color(1), const Color& colorB = Color(1), const Color& colorC = Color(1), const Color& colorD = Color(1), const Vec2f& uvA = Vec2f(0, 0), const Vec2f& uvB = Vec2f(1, 0), const Vec2f& uvC = Vec2f(1, 1), const Vec2f& uvD = Vec2f(0, 1)) {
		Path::batch->pushVertices({ { a, uvA, colorA }, { b, uvB, colorB }, { c, uvC, colorC }, { d, uvD, colorD } });
		Path::batch->pushIndices({ 0, 1, 2, 2, 3, 0 });
		Path::batch->endIndex();
	}

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch
	//? Expects vertices in counter-clockwise order
	void pushTriangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Color& colorA = Color(1), const Color& colorB = Color(1), const Color& colorC = Color(1), const Vec2f& uvA = Vec2f(0, 0), const Vec2f& uvB = Vec2f(1, 0), const Vec2f& uvC = Vec2f(0.5, 1)) {
		Path::batch->pushVertices({ { a, uvA, colorA }, { b, uvB, colorB }, { c, uvC, colorC } });
		Path::batch->pushIndices({ 0, 1, 2 });
		Path::batch->endIndex();
	}

	// Adds the vertices to the batch with the necessary indices, this does not reserve space on the batch
	void pushLine(const Vec2f& a, const Vec2f& b, const Color& colorA, const Color& colorB, float thickness) {
		Vec2f dxy = normalize(Vec2f(b.y - a.y, a.x - b.x)) / GUI::windowInfo.dimension.y * 3 * thickness;

		pushQuad(Vec2f(a + dxy), Vec2f(b + dxy), Vec2f(b - dxy), Vec2f(a - dxy), colorA, colorB, colorB, colorA);
	}

	#pragma endregion

	#pragma region Primitives

	//! Primitives

	void line(const Vec2f& a, const Vec2f& b, const Color& color, float thickness) {
		line(a, b, color, color, thickness);
	}

	void line(const Vec2f& a, const Vec2f& b, const Color& colorA, const Color& colorB, float thickness) {
		size_t vertexCount = 4;
		size_t indexCount = 6;
		Path::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, colorA, colorB, thickness);
	}

	void circle(const Vec2f& center, float radius, const Color& color, float thickness, size_t precision) {
		size_t vertexCount = 4 * precision;
		size_t indexCount = 6 * precision;
		Path::batch->reserve(vertexCount, indexCount);

		Vec2f oldPoint = center + Vec2f(radius, 0);
		float step = 2.0 * PI / (float) precision;
		for (size_t i = 1; i <= precision; i++) {
			float angle = i * step;

			Vec2f newPoint = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));

			pushLine(oldPoint, newPoint, color, color, thickness);

			oldPoint = newPoint;
		}
	}

	void circleFilled(const Vec2f& center, float radius, const Color& color, size_t precision) {
		size_t vertexCount = precision;
		size_t indexCount = 3 * (precision - 2);
		Path::batch->reserve(vertexCount, indexCount);
				
		float step = 2.0f * PI / (float) precision;
		for (size_t i = 0; i < precision; i++) {
			float angle = i * step;

			Vec2f point = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));
			Path::batch->pushVertex({ point, Vec2(1), color });
		}

		for (size_t i = 1; i < precision - 1; i++)
			Path::batch->pushIndices({ 0, i + 1, i });

		Path::batch->endIndex();
	}

	void circleSegment(const Vec2f& center, float radius, float minAngle, float maxAngle, bool sides, const Color& color, float thickness, size_t precision) {
		size_t vertexCount = 4 * (precision + (sides? 2 : 0));
		size_t indexCount = 6 * (precision + (sides? 2 : 0));
		Path::batch->reserve(vertexCount, indexCount);
		
		Vec2f oldPoint = Vec2f(center.x + radius * cos(minAngle), center.y + radius * sin(minAngle));
		
		if (sides)
			pushLine(center, oldPoint, color, color, thickness);

		float step = (maxAngle - minAngle) / (float)precision;
		for (size_t i = 1; i <= precision; i++) {
			float angle = minAngle + i * step;

			Vec2f newPoint = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));

			pushLine(oldPoint, newPoint, color, color, thickness);

			oldPoint = newPoint;
		}

		if (sides)
			pushLine(center, oldPoint, color, color, thickness);
	}

	void circleSegmentFilled(const Vec2f& center, float radius, float minAngle, float maxAngle, const Color& color, size_t precision) {
		size_t vertexCount = precision + 2;
		size_t indexCount = 3 * precision;
		Path::batch->reserve(vertexCount, indexCount);
		
		Vec2f oldPoint = Vec2f(center.x + radius * cos(minAngle), center.y + radius * sin(minAngle));
		
		Path::batch->pushVertex({ center, Vec2f(1.0f), color });
		Path::batch->pushVertex({ oldPoint, Vec2f(1.0f), color });

		float step = (maxAngle - minAngle) / (float) precision;
		for (size_t i = 1; i <= precision; i++) {
			float angle = minAngle + i * step;

			Vec2f newPoint = Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle));

			Path::batch->pushVertex({ newPoint, Vec2(1), color });
		}

		for (size_t i = 1; i <= precision + 1; i++)
			Path::batch->pushIndices({ 0, i + 1, i });
		
		Path::batch->endIndex();
	}

	void triangle(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Color& color, float thickness) {
		size_t vertexCount = 4 * 3;
		size_t indexCount = 6 * 3;
		Path::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, color, color, thickness);
		pushLine(b, c, color, color, thickness);
		pushLine(c, a, color, color, thickness);
	}

	void triangleFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Color& color) {
		size_t vertexCount = 3;
		size_t indexCount = 3;
		Path::batch->reserve(vertexCount, indexCount);

		pushTriangle(a, b, c, color, color, color);
	}

	void rect(const Vec2f& pos, const Vec2f& dim, float rounding, const Color& color, float thickness) {
		//if (rounding == 0.0) {
			quad(pos, pos + Vec2f(dim.x, 0), pos + Vec2f(dim.x, dim.y), pos + Vec2f(0, dim.y), color, thickness);
		//}
		// TODO add rounding
	}

	void rectFilled(const Vec2f& pos, const Vec2f& dim, float rounding, const Color& color) {
		//if (rounding == 0) {
			size_t vertexCount = 4;
			size_t indexCount = 6;
			Path::batch->reserve(vertexCount, indexCount);

			pushQuad(pos, pos + Vec2f(dim.x, 0), pos + Vec2f(dim.x, dim.y), pos + Vec2f(0, dim.y), color, color, color, color);
		//}
		// TODO add rounding
	}

	void rectUV(GLID id, const Vec2f& pos, const Vec2f& dim, const Vec2f& uvMin, const Vec2f& uvMax, const Color& color) {
		size_t vertexCount = 4;
		size_t indexCount = 6;

		Vec2f a = pos;
		Vec2f b = Vec2f(pos.x + dim.x, pos.y);
		Vec2f c = Vec2f(pos.x + dim.x, pos.y + dim.y);
		Vec2f d = Vec2f(pos.x, pos.y + dim.y);
		Vec2f uvA = uvMin;
		Vec2f uvB = Vec2f(uvMax.x, uvMin.y);
		Vec2f uvC = uvMax;
		Vec2f uvD = Vec2f(uvMin.x, uvMax.y);

		Path::batch->pushCommand(0);
		Path::batch->reserve(vertexCount, indexCount);
		pushQuad(a, b, c, d, color, color, color, color, uvA, uvB, uvC, uvD);
		Path::batch->pushCommand(id);
	}

	void rectUVRange(GLID id, const Vec2f& pos, const Vec2f& dim, const Vec2f& xRange, const Vec2f& yRange, const Color& color) {
		size_t vertexCount = 4;
		size_t indexCount = 6;
		
		float dx = xRange.y - xRange.x;
		float dy = yRange.y - yRange.x;

		float u = (pos.x - xRange.x) / dx;
		float v = (pos.y - dim.y - yRange.x) / dy;
		float du = dim.x / dx;
		float dv = dim.y / dy;

		Vec2f a = pos;
		Vec2f b = Vec2f(pos.x + dim.x, pos.y);
		Vec2f c = Vec2f(pos.x + dim.x, pos.y - dim.y);
		Vec2f d = Vec2f(pos.x, pos.y - dim.y);
		Vec2f uvA = Vec2f(u, v + dv);
		Vec2f uvB = Vec2f(u + du, v + dv);
		Vec2f uvC = Vec2f(u + du, v);
		Vec2f uvD = Vec2f(u, v);

		Path::batch->pushCommand(0);
		Path::batch->reserve(vertexCount, indexCount);
		pushQuad(a, b, c, d, color, color, color, color, uvA, uvB, uvC, uvD);
		Path::batch->pushCommand(id);
	}

	void quad(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Color& color, float thickness) {
		size_t vertexCount = 4 * 4;
		size_t indexCount = 6 * 4;
		Path::batch->reserve(vertexCount, indexCount);

		pushLine(a, b, color, color, thickness);
		pushLine(b, c, color, color, thickness);
		pushLine(c, d, color, color, thickness);
		pushLine(d, a, color, color, thickness);
	}

	void quadFilled(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Color& color) {
		size_t vertexCount = 4;
		size_t indexCount = 6;
		Path::batch->reserve(vertexCount, indexCount);

		pushQuad(a, b, c, d, color, color, color, color);
	}

	void quadUV(GLID id, const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Vec2f& uvA, const Vec2f& uvB, const Vec2f& uvC, const Vec2f& uvD) {
		size_t vertexCount = 4;
		size_t indexCount = 6;
		Path::batch->pushCommand(0); 
		Path::batch->reserve(vertexCount, indexCount);
		pushQuad(a, b, c, d, Color(1), Color(1), Color(1), Color(1), uvA, uvB, uvC, uvD);
		Path::batch->pushCommand(id);
	}

	void text(Font* font, const std::string& text, double size, const Vec2f& pos, const Color& color, char textPivot) {
		if (text.empty())
			return;

		size_t vertexCount = 4 * text.size();
		size_t indexCount = 6 * text.size();
		Vec2f textSize = font->size(text, size);

		// TextPivotHL default
		float x = pos.x;

		// TextPivotVB default
		float y = pos.y;

		if (textPivot & TextPivotHC)
			x -= textSize.x / 2.0f;

		if (textPivot & TextPivotHR)
			x -= textSize.x;

		if (textPivot & TextPivotVT)
			y -= textSize.y;

		if (textPivot & TextPivotVC)
			y -= textSize.y / 2.0f;

		Path::batch->pushCommand(0);
		Path::batch->reserve(vertexCount, indexCount);
		for (char c : text) {
			int ascii = (int) c;
			const Character& character = font->getCharacter(ascii);
			float descend = character.height - character.by;
			float xpos = x + character.bx * size;
			float ypos = y - descend * size;

			float w = character.width * size;
			float h = character.height * size;

			float s = float(character.x) / font->getAtlasWidth();
			float t = float(character.y) / font->getAtlasHeight();
			float ds = float(character.width) / font->getAtlasWidth();
			float dt = float(character.height) / font->getAtlasHeight();

			Vec2f a = Vec2f(xpos, ypos);
			Vec2f b = Vec2f(xpos + w, ypos);
			Vec2f c = Vec2f(xpos + w, ypos + h);
			Vec2f d = Vec2f(xpos, ypos + h);
			Vec2f uvA = Vec2f(s, t + dt);
			Vec2f uvB = Vec2f(s + ds, t + dt);
			Vec2f uvC = Vec2f(s + ds, t);
			Vec2f uvD = Vec2f(s, t);

			pushQuad(a, b, c, d, color, color, color, color, uvA, uvB, uvC, uvD);

			x += (character.advance >> 6) * size;
		}

		Path::batch->pushCommand(font->getAtlasID());
	}

	void bezier(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, const Color& color, float thickness, size_t precision) {
		bezier(a, b, c, d, DEFAULT_PATTERN_2D(color), thickness, precision);
	}

	void bezier(const Vec2f& a, const Vec2f& b, const Vec2f& c, const Vec2f& d, Pattern2D pattern, float thickness, size_t precision) {
		size_t vertexCount = 4 * precision;
		size_t indexCount = 6 * precision;
		Path::batch->reserve(vertexCount, indexCount);

		Vec2f oldPoint = a;
		Color oldColor = pattern(0, oldPoint);
		float step = 1.0f / (float)precision;
		for (size_t i = 1; i <= precision; i++) {
			float t = i * step;
			float s = 1 - t;

			float w1 = s * s * s;
			float w2 = 3.0f * s * s * t;
			float w3 = 3.0f * s * t * t;
			float w4 = t * t * t;

			Vec2f newPoint = w1 * a + w2 * b + w3 * c + w4 * d;
			Color newColor = pattern(i, newPoint);
			pushLine(oldPoint, newPoint, oldColor, newColor, thickness);

			oldPoint = newPoint;
			oldColor = newColor;
		}
	}

	void bezierHorizontal(const Vec2f& start, const Vec2f& end, const Color& color, float thickness, size_t precision) {
		bezierHorizontal(start, end, DEFAULT_PATTERN_2D(color), thickness, precision);
	}

	void bezierHorizontal(const Vec2f& start, const Vec2f& end, Pattern2D pattern, float thickness, size_t precision) {
		float mid = (start.x + end.x) / 2.0f;
		Vec2f c1 = Vec2f(mid, start.y);
		Vec2f c2 = Vec2f(mid, end.y);
		bezier(start, c1, c2, end, pattern, thickness, precision);
	}

	void bezierVertical(const Vec2f& start, const Vec2f& end, const Color& color, float thickness, size_t precision) {
		bezierVertical(start, end, DEFAULT_PATTERN_2D(color), thickness, precision);
	}

	void bezierVertical(const Vec2f& start, const Vec2f& end, Pattern2D pattern, float thickness, size_t precision) {
		float mid = (start.y + end.y) / 2.0f;
		Vec2f c1 = Vec2f(start.x, mid);
		Vec2f c2 = Vec2f(end.x, mid);
		bezier(start, c1, c2, end, pattern, thickness, precision);
	}

	void polyLine(Vec2f* points, size_t size, const Color& color, float thickness, bool closed) {
		polyLine(points, size, DEFAULT_PATTERN_2D(color), thickness, closed);
	}

	void polyLine(Vec2f* points, size_t size, Pattern2D pattern, float thickness, bool closed) {
		if (size == 0)
			return; 

		if (size == 1)
			return; // Points not supported yet

		size_t vertexCount = 4 * (size - (closed? 0 : 1));
		size_t indexCount = 6 * (size - (closed ? 0 : 1));

		Path::batch->reserve(vertexCount, indexCount);

		Color oldColor = pattern(0, points[0]);
		for (size_t i = 0; i < size - 1; i++) {
			Color newColor = pattern(i + 1, points[i + 1]);
			pushLine(points[i], points[i + 1], oldColor, newColor, thickness);
			oldColor = newColor;
		}
		
		if (closed)
			pushLine(points[size - 1], points[0], oldColor, pattern(0, points[0]), thickness);
	}

	void polygonFilled(Vec2f* points, size_t size, const Color& color) {
		polygonFilled(points, size, DEFAULT_PATTERN_2D(color));
	}

	void polygonFilled(Vec2f* points, size_t size, Pattern2D pattern) {
		if (size == 0) {
			return;
		}

		if (size == 1) {
			// point not supported
			return;
		}

		if (size == 2) {
			line(points[0], points[1], pattern(0, points[0]), pattern(1, points[1]), 1.0f);
			return;
		}

		size_t vertexCount = size;
		size_t indexCount = 3 * (size - 2);

		Path::batch->reserve(vertexCount, indexCount);

		for (size_t i = 0; i < size; i++)
			Path::batch->pushVertex({ points[i], Vec2f(1), pattern(i, points[i]) });

		for (size_t i = 0; i < size - 2; i++)
			Path::batch->pushIndices({ 0, i + 1, i + 2 });

		Path::batch->endIndex();
	}
	
	void catmullRom(Vec2f* points, size_t size, const Color& color, int precision, float thickness, bool closed, float tension, float alpha) {
		catmullRom(points, size, DEFAULT_PATTERN_2D(color), precision, thickness, closed, tension, alpha);
	}

	void catmullRom(Vec2f* points, size_t size, Pattern2D pattern, int precision, float thickness, bool closed, float tension, float alpha) {
		// Checks
		if (tension == 1.0f)
			polyLine(points, size, pattern, thickness, closed);

		if (size == 0)
			return;

		if (size == 1)
			return; // Points not supported yet

		if (size == 2)
			line(points[0], points[1], pattern(0, points[0]), pattern(1, points[1]), 1.0f);


		// Build lists
		Vec2f* pList = (Vec2f*) alloca((size + 1) * sizeof(Vec2f));
		float* tList = (float*) alloca((size + 1) * sizeof(float));

		Vec2 start = points[0] * 2 - points[1];
		Vec2 end = points[size - 1] * 2 - points[size - 2];
		float alpha2 = alpha / 2.0f;

		pList[0] = points[0] - start;
		tList[0] = pow(lengthSquared(pList[0]), alpha2);

		for (size_t i = 1; i < size; i++) {
			pList[i] = points[i] - points[i - 1];
			tList[i] = pow(lengthSquared(pList[i]), alpha2);
		}

		pList[size] = end - points[size - 1];
		tList[size] = pow(lengthSquared(pList[size]), alpha2);


		// Reserve vertices
		size_t vertexCount = 4 * size * precision;
		size_t indexCount = 6 * size * precision;

		Path::batch->reserve(vertexCount, indexCount);

		float s = 1.0f - tension;
		float step = 1.0f / (float)precision;

		for (size_t i = 1; i < size; i++) {
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
			Color oldColor = pattern((i - 1) * precision, oldPoint);
			for (size_t j = 1; j <= precision; j++) {
				float t = j * step;
				float w4 = 1.0f;
				float w3 = w4 * t;
				float w2 = w3 * t;
				float w1 = w2 * t;

				Vec2f newPoint = a * w1 + b * w2 + c * w3 + d * w4;
				Color newColor = pattern((i - 1) * precision + j, newPoint);

				pushLine(oldPoint, newPoint, oldColor, newColor, thickness);

				oldPoint = newPoint;
				oldColor = newColor;
			}
		}
	}

	#pragma endregion

	#pragma region Path

	//! Path

	std::vector<Vec2f> path;

	void lineTo(const Vec2f& vertex) {
		path.push_back(vertex);
	}

	void arcTo(const Vec2f& center, float radius, float minAngle, float maxAngle, size_t precision) {
		if (precision == 0 || radius == 0.0f) {
			path.push_back(center);
			return;
		}

		path.reserve(path.size() + precision + 1);

		for (size_t i = 0; i <= precision; i++) {
			float angle = minAngle + ((float)i / (float)precision) * (maxAngle - minAngle);
			path.push_back(Vec2f(center.x + radius * cos(angle), center.y + radius * sin(angle)));
		}
	}

	void bezierTo(const Vec2f& end, const Vec2f& tc1, const Vec2f& tc2, size_t precision) {
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

	void bezierTo(const Vec2f& end, size_t precision) {
		Vec2f start = path.back();

		float mid = (start.y + end.y) / 2.0f;
		Vec2f c1 = Vec2f(start.x, mid);
		Vec2f c2 = Vec2f(end.x, mid);

		bezierTo(end, c1, c2, precision);
	}

	void stroke(const Color& color, float thickness, bool closed) {
		stroke(DEFAULT_PATTERN_2D(color), thickness, closed);
	}

	void stroke(Pattern2D pattern, float thickness, bool closed) {
		polyLine(path.data(), path.size(), pattern, thickness, closed);

		clear();
	}

	void fill(const Color& color) {
		fill(DEFAULT_PATTERN_2D(color));
	}

	void fill(Pattern2D pattern) {
		polygonFilled(path.data(), path.size(), pattern);

		clear();
	}

	int size() {
		return path.size();
	}

	void clear() {
		path.clear();
	}

	#pragma endregion

}

};