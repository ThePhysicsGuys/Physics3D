#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "arrayMesh.h"
#include "../graphics/renderer.h"
#include "../physics/math/linalg/vec.h"
#include "../physics/math/mathUtil.h"

namespace Graphics {

struct Primitive {
	unsigned int vao;
	unsigned int vbo;
	int M;
	int N;

	template<int M, int N>
	void resize(float(&vertices)[M][N]) {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, M * N * sizeof(float), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	virtual void render() = 0;

protected:
	// M is size of vertex, N is amount of vertices
	Primitive(int M, int N) {
		this->M = M;
		this->N = N;

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * M * N, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, M, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	};
};


// Quad

struct Quad : public Primitive {

	Quad() : Primitive(4, 4) {
		resize(Vec2f(-1, 1), Vec2f(2));
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
		Renderer::polygonMode(Renderer::FRONT_AND_BACK, mode);
		
		glBindVertexArray(vao);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);

		Renderer::polygonMode(Renderer::FRONT_AND_BACK, Renderer::FILL);
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
		resize(Vec3f(-1), Vec3f(1));
	}

	void render(int mode) {
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		glBindVertexArray(vao);
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void render() override {
		render(GL_FILL);
	}
};


// Triangle

struct TrianglePrimitive : public Primitive {

	bool patched;

	TrianglePrimitive(bool patched = false) : Primitive(3, 3) {
		this->patched = patched;
		resize(Vec3f(-1), Vec3f(1), Vec3f(0, 1, -1));
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
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		glBindVertexArray(vao);
		glDrawArrays((patched) ? GL_PATCHES : GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void render() override {
		render(GL_FILL);
	}
};

};
