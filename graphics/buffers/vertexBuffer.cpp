#include "core.h"

#include "vertexBuffer.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

namespace Graphics {

VertexBuffer::VertexBuffer() {
	id = 0;
	Log::debug("Created empty vertex buffer");
};

VertexBuffer::VertexBuffer(const void* data, size_t sizeInBytes, unsigned int mode) : Bindable() {
	glGenBuffers(1, &id);
	bind();
	if (sizeInBytes != 0)
		fill(data, sizeInBytes, mode);
}

VertexBuffer::VertexBuffer(const void* data, size_t sizeInBytes) : VertexBuffer(data, sizeInBytes, GL_STATIC_DRAW) {}


VertexBuffer::~VertexBuffer() {
	close();
	Log::warn("Deleted vertex buffer with id (%d)", id);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) {
	id = other.id;
	other.id = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
	}

	return *this;
}

void VertexBuffer::fill(const void* data, size_t sizeInBytes, unsigned int mode) {
	bind();
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, mode);
}

void VertexBuffer::update(const void* data, size_t sizeInBytes, int offset) {
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, data);
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
	id = 0;
}

};