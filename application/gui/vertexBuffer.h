#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "../../util/log.h"

class VertexBuffer {
private:
	unsigned int id;

public:
	VertexBuffer() : id(0) {
		Log::debug("Created empty vertex buffer");
	};

	VertexBuffer(const double* data, unsigned int size, unsigned int mode) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(double), data, mode);
		Log::debug("Created vertex buffer width id %d, size %d, data %d", id, size, data);
	}

	VertexBuffer(const double* data, unsigned int size) : VertexBuffer(data, size, GL_STATIC_DRAW) {}

	~VertexBuffer() {
		close();
		Log::warn("Deleted vertex buffer with id %d", id);
	}

	void bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void close() {
		unbind();
		glDeleteBuffers(1, &id);
	}
};
