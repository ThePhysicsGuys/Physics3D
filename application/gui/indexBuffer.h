#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

class IndexBuffer {
private:
	unsigned int id;

public:
	IndexBuffer(const unsigned int* data, unsigned int size) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	IndexBuffer() {
		glDeleteBuffers(1, &id);
	}

	void bind() {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};