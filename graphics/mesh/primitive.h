#pragma once

#include "../graphics/renderer.h"
#include <Physics3D/math/linalg/vec.h>

namespace P3D::Graphics {

struct Primitive {
	unsigned int vao;
	unsigned int vbo;
	int M;
	int N;

	template<int M, int N>
	void resize(float(&vertices)[M][N]) {
		using namespace Renderer;
		
		bindVertexArray(vao);
		bindBuffer(ARRAY_BUFFER, vbo);
		bufferSubData(ARRAY_BUFFER, 0, M * N * sizeof(float), vertices);
		bindBuffer(ARRAY_BUFFER, 0);
		bindVertexArray(0);
	}

	virtual void render() = 0;

protected:
	// M is size of vertex, N is amount of vertices
	Primitive(int M, int N) {
		using namespace Renderer;
		
		this->M = M;
		this->N = N;

		genVertexArrays(1, &vao);
		genBuffers(1, &vbo);

		bindVertexArray(vao);
		bindBuffer(ARRAY_BUFFER, vbo);

		bufferData(ARRAY_BUFFER, sizeof(float) * M * N, 0, DYNAMIC_DRAW);

		enableVertexAttribArray(0);
		vertexAttribPointer(0, M, FLOAT, false, 0, 0);

		bindBuffer(ARRAY_BUFFER, 0);
		bindVertexArray(0);
	}

	~Primitive() {
		using namespace Renderer;

		delBuffers(1, &vbo);
		delVertexArrays(1, &vao);
	}
};


// Quad

struct Quad : public Primitive {

	Quad() : Primitive(4, 4) {
		resize(Vec2f(-1, 1), Vec2f(2.0f, 2.0f));
	}

	Quad(Vec2f topLeft, Vec2f dimension) : Primitive(4, 4) {
		resize(topLeft, dimension);
	}

	void resize(Vec2f position, Vec2f dimension, Vec2f xRange, Vec2f yRange) {
		float dx = xRange.y - xRange.x;
		float dy = yRange.y - yRange.x;

		float u = (position.x - xRange.x) / dx;
		float v = (position.y - dimension.y - yRange.x) / dy;
		float du = dimension.x / dx;
		float dv = dimension.y / dy;

		float vertices[4][4] = {
			{ position.x,				position.y				, u,      v + dv},
			{ position.x,				position.y - dimension.y, u,      v     },
			{ position.x + dimension.x, position.y - dimension.y, u + du, v     },
			{ position.x + dimension.x, position.y				, u + du, v + dv}
		};

		Primitive::resize(vertices);
	}

	void resize(Vec2f position, Vec2f dimension) {

		float vertices[4][4] = {
			{ position.x,				position.y				, 0.0f, 1.0f},
			{ position.x,				position.y - dimension.y, 0.0f, 0.0f},
			{ position.x + dimension.x, position.y - dimension.y, 1.0f, 0.0f},
			{ position.x + dimension.x, position.y				, 1.0f, 1.0f}
		};

		Primitive::resize(vertices);
	}

	void render(int mode) {
		using namespace Renderer;

		polygonMode(FRONT_AND_BACK, mode);

		bindVertexArray(vao);
		drawArrays(QUADS, 0, 4);
		bindVertexArray(0);

		polygonMode(FRONT_AND_BACK, FILL);
	}

	void render() override {
		render(Renderer::FILL);
	}
};


// Line

struct LinePrimitive : public Primitive {

	void resize(Vec3f p1, Vec3f p2) {
		float vertices[2][3] = {
			{ p1.x,	p1.y, p1.z },
			{ p2.x,	p2.y, p2.z }
		};

		Primitive::resize(vertices);
	}

	void resize(Vec2f p1, Vec2f p2) {
		float vertices[2][3] = {
			{ p1.x,	p1.y, 0 },
			{ p2.x,	p2.y, 0 }
		};

		Primitive::resize(vertices);
	}

	LinePrimitive() : Primitive(3, 2) {
		resize(Vec3f::full(-1.0f), Vec3f::full(1.0f));
	}

	void render(int mode) {
		using namespace Renderer;
		
		polygonMode(FRONT_AND_BACK, mode);

		bindVertexArray(vao);
		drawArrays(LINE_STRIP, 0, 2);
		bindVertexArray(0);

		polygonMode(FRONT_AND_BACK, FILL);
	}

	void render() override {
		render(Renderer::FILL);
	}
};


// Triangle

struct TrianglePrimitive : public Primitive {

	bool patched;

	TrianglePrimitive(bool patched = false) : Primitive(3, 3) {
		this->patched = patched;
		resize(Vec3f::full(-1.0f), Vec3f::full(1.0f), Vec3f(0.0f, 1.0f, -1.0f));
	}

	void resize(Vec3f p1, Vec3f p2, Vec3f p3) {
		float vertices[3][3] = {
			{ p1.x,	p1.y, p1.z },
			{ p2.x,	p2.y, p2.z },
			{ p3.x,	p3.y, p3.z }
		};

		Primitive::resize(vertices);
	}

	void resize(Vec2f p1, Vec2f p2, Vec2f p3) {

		float vertices[3][3] = {
			{ p1.x,	p1.y, 0 },
			{ p2.x,	p2.y, 0 },
			{ p3.x,	p3.y, 0 }
		};

		Primitive::resize(vertices);
	}

	void render(int mode) {
		using namespace Renderer;

		polygonMode(FRONT_AND_BACK, mode);

		bindVertexArray(vao);
		drawArrays(patched ? PATCHES : TRIANGLES, 0, 3);
		bindVertexArray(0);

		polygonMode(FRONT_AND_BACK, FILL);
	}

	void render() override {
		render(Renderer::FILL);
	}
};

};
