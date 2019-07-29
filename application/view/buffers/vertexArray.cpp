#include "vertexArray.h"

#include "GL\glew.h"

#include "vertexBuffer.h"
#include "bufferLayout.h"

#include "../util/log.h"

VertexArray::VertexArray() : attribArrayOffset(0) {
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
}

VertexArray::~VertexArray() {
	close();
	Log::debug("Deleted vertex array with id (%d)", id);
}

void VertexArray::bind() {
	glBindVertexArray(id);
	for (unsigned int i = 0; i < attribArrayOffset; i++)
		glEnableVertexAttribArray(i);
}

void VertexArray::unbind() {
	glBindVertexArray(0);
}

void VertexArray::addBuffer(VertexBuffer& buffer, const BufferLayout& layout) {
	bind();
	buffer.bind();
	size_t offset = 0;
	for (int i = 0; i < layout.elements.size(); i++) {
		auto& element = layout.elements[i];
		glEnableVertexAttribArray(attribArrayOffset + i);
		glVertexAttribPointer(attribArrayOffset + i, element.count, element.type, element.normalized, layout.stride, (const void*)offset);
		offset += element.count * element.size;
	}
	attribArrayOffset += (unsigned int)layout.elements.size();
}

void VertexArray::close() {
	unbind();
	glDeleteVertexArrays(1, &id);
}