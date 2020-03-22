#pragma once

#include "../bindable.h"
#include "../renderer.h"

namespace Graphics {

class VertexBuffer : public Bindable {
public:
	VertexBuffer();
	VertexBuffer(const void* data, size_t sizeInBytes, GLFLAG mode);
	VertexBuffer(const void* data, size_t sizeInBytes);

	~VertexBuffer();
	VertexBuffer(VertexBuffer&& other);
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(VertexBuffer&& other);
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	void fill(const void* data, size_t sizeInBytes, GLFLAG mode);
	void update(const void* data, size_t sizeInBytes, int offset);

	void bind() override;
	void unbind() override;
	void close() override;
};


};