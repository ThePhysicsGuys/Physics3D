#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

class VertexBuffer {
private:
	unsigned int id;

public:
	VertexBuffer(const double* data, unsigned int size) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(double), data, GL_STATIC_DRAW);
	}

	~VertexBuffer() {
		glDeleteBuffers(1, &id);
	}

	void bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};
