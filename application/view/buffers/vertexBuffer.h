#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../bindable.h"

#include "../util/log.h"

class VertexBuffer : public Bindable {
public:
	VertexBuffer() {
		id = 0;
		Log::debug("Created empty vertex buffer");
	};

	VertexBuffer(const float* data, size_t size, unsigned int mode) : Bindable() {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, mode);
	}
	
	VertexBuffer(const float* data, size_t size) : VertexBuffer(data, size, GL_STATIC_DRAW) {}

	~VertexBuffer() {
		close();
		Log::warn("Deleted vertex buffer with id (%d)", id);
	}

	void bind() override {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void unbind() override {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void close() override {
		unbind();
		glDeleteBuffers(1, &id);
	}
};
