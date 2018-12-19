#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

class IndexBuffer {
private:
	unsigned int id;

public:
	IndexBuffer() : id(0) {
		Log::debug("Created empty index buffer");
	};

	IndexBuffer(const unsigned int* data, unsigned int size) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, GL_STATIC_DRAW);
		Log::debug("Created index buffer width id, (%d), size (%d) and datapointer (%d)", id, size, data);
	}

	~IndexBuffer() {
		close();
		Log::warn("Deleted index buffer with id (%d)", id);
	}

	void bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	void unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void close() {
		unbind();
		glDeleteBuffers(1, &id);
	}
};