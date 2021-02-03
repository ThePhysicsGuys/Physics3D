#include "core.h"

#include "vertexBuffer.h"

#include <GL/glew.h>

namespace P3D::Graphics {

VertexBuffer::VertexBuffer() : Bindable(), currentSize(0), currentCapacity(0), layout(BufferLayout()) {
	Log::debug("Created empty vertex buffer");
};

VertexBuffer::VertexBuffer(const BufferLayout& layout, const void* data, size_t sizeInBytes, GLFLAG mode) : Bindable(), currentSize(sizeInBytes), currentCapacity(sizeInBytes), layout(layout) {
	glGenBuffers(1, &id);

	bind();
	if (sizeInBytes != 0)
		fill(data, sizeInBytes, mode);
}

VertexBuffer::VertexBuffer(const BufferLayout& layout, const void* data, size_t sizeInBytes) : VertexBuffer(layout, data, sizeInBytes, GL_STATIC_DRAW) {}


VertexBuffer::~VertexBuffer() {
	close();
	Log::warn("Deleted vertex buffer with id (%d)", id);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) :
	Bindable(other.id),
	currentSize(other.currentSize), 
	currentCapacity(other.currentCapacity),
	layout(std::move(other.layout)) {

	// Reset to prevent destruction by rvalue
	other.id = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
		std::swap(layout, other.layout);
		std::swap(currentSize, other.currentSize);
		std::swap(currentCapacity, other.currentCapacity);
	}

	return *this;
}

void VertexBuffer::fill(const void* data, std::size_t sizeInBytes, GLFLAG mode) {
	currentSize = sizeInBytes;
	currentCapacity = sizeInBytes;

	if (currentSize == 0)
		return;

	bind();
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, mode);
}

void VertexBuffer::update(const void* data, std::size_t sizeInBytes, std::size_t offset) {
	size_t newCapacity = offset + sizeInBytes;
	
	bind();
	if (newCapacity > currentCapacity) {
		Log::error("Buffer update exceeds buffer capacity: buffer=%d, size=%d, capacity=%d", id, sizeInBytes + offset, currentCapacity);
		return;
	}

	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeInBytes, data);
}

size_t VertexBuffer::size() const {
	return currentSize;
}

size_t VertexBuffer::capacity() const {
	return currentCapacity;
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
	layout = BufferLayout();
	currentCapacity = 0;
	currentCapacity = 0;
}

};