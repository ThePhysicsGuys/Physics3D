#pragma once

#include "arrayMesh.h"
#include "../../engine/math/vec2.h"

class Quad {
private:
	unsigned int vao;
	unsigned int vbo;

public:

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

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void resize(Vec2f position, Vec2f dimension) {

		float vertices[4][4] = {
			{ position.x,				position.y				, 0, 1},
			{ position.x,				position.y - dimension.y, 0, 0},
			{ position.x + dimension.x, position.y - dimension.y, 1, 0},
			{ position.x + dimension.x, position.y				, 1, 1}
		};

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Quad() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		resize(Vec2(-1, 1), Vec2(2));
	}

	void render(int mode) {
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		glBindVertexArray(vao);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void render() {
		render(GL_FILL);
	}
};