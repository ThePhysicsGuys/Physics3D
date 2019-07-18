#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../util/log.h"

#include "../bindable.h"

class IndexBuffer : public Bindable {
public:
	IndexBuffer() {
		id = 0;
		Log::debug("Created empty index buffer");
	};

	IndexBuffer(const unsigned int* data, unsigned int size) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	~IndexBuffer() {
		close();
		Log::warn("Deleted index buffer with id (%d)", id);
	}

	void bind() override {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	void unbind() override {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void close() override {
		unbind();
		glDeleteBuffers(1, &id);
	}
};