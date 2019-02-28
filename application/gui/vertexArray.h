#pragma once

#include "GL\glew.h"

#include "vertexBuffer.h"
#include "bufferLayout.h"
#include "../../util/log.h"

class VertexArray {
public:
	unsigned int id;
	unsigned int attribArrayOffset;

	VertexArray() : attribArrayOffset(0) {
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
		Log::debug("Created vertex array with id (%d)", id);
	}

	~VertexArray() {
		close();
		Log::debug("Deleted vertex array with id (%d)", id);
	}

	void bind() const {
		glBindVertexArray(id);
		for (int i = 0; i < attribArrayOffset; i++)
			glEnableVertexAttribArray(i);
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
			glEnableVertexAttribArray(attribArrayOffset + i);
			glVertexAttribPointer(attribArrayOffset + i, element.count, element.type, element.normalized, layout.stride, (const void*) offset);
			offset += element.count * element.size;
		}
		attribArrayOffset += layout.elements.size();
	}

	void close() {
		unbind();
		glDeleteVertexArrays(1, &id);
	}
};