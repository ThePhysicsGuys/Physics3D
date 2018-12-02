#pragma once

#include "GL\glew.h"

#include "vertexBuffer.h"
#include "bufferLayout.h"

class VertexArray {
public:
	unsigned int id;

	VertexArray() {
		glGenVertexArrays(1, &id);
	}

	~VertexArray() {
		glDeleteVertexArrays(1, &id);
	}

	void bind() const {
		glBindVertexArray(id);
	}

	void unbind() const {
		glBindVertexArray(0);
	}

	void addBuffer(const VertexBuffer& buffer, const BufferLayout& layout) {
		bind();
		buffer.bind();
		unsigned int offset = 0;
		for (int i = 0; i < layout.elements.size(); i++) {
			auto& element = layout.elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride, (const void*) offset);
			offset += element.count * element.size;
		}
	}
};