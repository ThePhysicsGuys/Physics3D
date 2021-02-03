#pragma once

#include "../bindable.h"
#include "../renderer.h"
#include "bufferLayout.h"

namespace P3D::Graphics {

class VertexBuffer : public Bindable {
private:
	std::size_t currentSize;
	std::size_t currentCapacity;

public:
	BufferLayout layout;
	
	VertexBuffer();
	VertexBuffer(const BufferLayout& layout, const void* data, std::size_t sizeInBytes, GLFLAG mode);
	VertexBuffer(const BufferLayout& layout, const void* data, std::size_t sizeInBytes);

	~VertexBuffer();
	VertexBuffer(VertexBuffer&& other);
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(VertexBuffer&& other);
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	void fill(const void* data, std::size_t sizeInBytes, GLFLAG mode);
	void update(const void* data, std::size_t sizeInBytes, std::size_t offset);
	std::size_t size() const;
	std::size_t capacity() const;

	void bind() override;
	void unbind() override;
	void close() override;
};


};