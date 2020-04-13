#include "core.h"

#include "vertexBuffer.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

namespace Graphics {

VertexBuffer::VertexBuffer() {
	id = 0;
	Log::debug("Created empty vertex buffer");
};

VertexBuffer::VertexBuffer(const void* data, size_t sizeInBytes, GLFLAG mode) : Bindable(), _size(sizeInBytes), _capacity(sizeInBytes) {
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
		std::swap(_size, other._size);
		std::swap(_capacity, other._capacity);
	}

	return *this;
}

void VertexBuffer::fill(const void* data, size_t sizeInBytes, GLFLAG mode) {
	_size = sizeInBytes;
	_capacity = sizeInBytes;

	bind();
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, mode);
}

void VertexBuffer::update(const void* data, size_t sizeInBytes, int offset) {
	size_t newCapacity = offset + sizeInBytes;
	
	bind();
	if (newCapacity > _capacity) {
		Log::error("Buffer update exceeds buffer capacity: buffer=%d, size=%d, capacity=%d", id, sizeInBytes + offset, _capacity);
		return;
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeInBytes, (const void*) (intptr_t) data);
}

size_t VertexBuffer::size() const {
	return _size;
}

size_t VertexBuffer::capacity() const {
	return _capacity;
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