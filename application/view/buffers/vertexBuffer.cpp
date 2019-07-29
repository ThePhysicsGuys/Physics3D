#include "vertexBuffer.h"

VertexBuffer::VertexBuffer() {
	id = 0;
	Log::debug("Created empty vertex buffer");
};

VertexBuffer::VertexBuffer(const float* data, size_t size, unsigned int mode) : Bindable() {
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, mode);
}

VertexBuffer::VertexBuffer(const float* data, size_t size) : VertexBuffer(data, size, GL_STATIC_DRAW) {

}

VertexBuffer::~VertexBuffer() {
	close();
	Log::warn("Deleted vertex buffer with id (%d)", id);
}

void VertexBuffer::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::close() {
	unbind();
	glDeleteBuffers(1, &id);
}