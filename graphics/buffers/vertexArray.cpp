#include "core.h"

#include "vertexArray.h"

#include <GL/glew.h>

#include "vertexBuffer.h"
#include "bufferLayout.h"

namespace P3D::Graphics {

VertexArray::VertexArray() : attributeArrayOffset(0) {
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
}

VertexArray::~VertexArray() {
	close();
	Log::debug("Deleted vertex array with id (%d)", id);
}

VertexArray::VertexArray(VertexArray&& other) {
	attributeArrayOffset = other.attributeArrayOffset;
	other.attributeArrayOffset = 0;

	id = other.id;
	other.id = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
	}

	return *this;
}

void VertexArray::bind() {
	glBindVertexArray(id);
}

void VertexArray::unbind() {
	glBindVertexArray(0);
}

void VertexArray::addBuffer(VertexBuffer* buffer) {
	bind();
	buffer->bind();

	size_t offset = 0;
	for (size_t i = 0; i < buffer->layout.elements.size(); i++) {
		auto& element = buffer->layout.elements[i];

		int iterations = (element.info == BufferDataType::MAT2 || element.info == BufferDataType::MAT3 || element.info == BufferDataType::MAT4) ? element.info.count : 1;

		for (size_t j = 0; j < iterations; j++) {
			glEnableVertexAttribArray(attributeArrayOffset);
			if (element.info.integral)
				glVertexAttribIPointer(attributeArrayOffset, element.info.count, element.info.type, buffer->layout.stride, (const void*) offset);
			else
				glVertexAttribPointer(attributeArrayOffset, element.info.count, element.info.type, element.normalized, buffer->layout.stride, (const void*) offset);

			if (element.instanced)
				glVertexAttribDivisor(attributeArrayOffset, 1);

			offset += element.info.size;
			attributeArrayOffset++;
		}
	}
}

void VertexArray::close() {
	unbind();
	glDeleteVertexArrays(1, &id);
	id = 0;
}

};