#pragma once

#include "GL\glew.h"

#include "vertexBuffer.h"
#include "bufferLayout.h"
#include "bindable.h"

#include "../../util/log.h"

class VertexArray : public Bindable {
public:
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

	void bind() override {
		glBindVertexArray(id);
		for (unsigned int i = 0; i < attribArrayOffset; i++)
			glEnableVertexAttribArray(i);
	}

	void unbind() override {
		glBindVertexArray(0);
	}

	void addBuffer(VertexBuffer& buffer, const BufferLayout& layout) {
		bind();
		buffer.bind();
		size_t offset = 0;
		for (int i = 0; i < layout.elements.size(); i++) {
			auto& element = layout.elements[i];
			glEnableVertexAttribArray(attribArrayOffset + i);
			glVertexAttribPointer(attribArrayOffset + i, element.count, element.type, element.normalized, layout.stride, (const void*) offset);
			offset += element.count * element.size;
		}
		attribArrayOffset += (unsigned int) layout.elements.size();
	}

	void close() override {
		unbind();
		glDeleteVertexArrays(1, &id);
	}
};