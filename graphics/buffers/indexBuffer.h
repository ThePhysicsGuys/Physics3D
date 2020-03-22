#pragma once

#include "../bindable.h"
#include "../renderer.h"

namespace Graphics {

class IndexBuffer : public Bindable {
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, size_t size, GLFLAG mode);
	IndexBuffer(const unsigned int* data, size_t size);

	~IndexBuffer();
	IndexBuffer(IndexBuffer&& other);
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(IndexBuffer&& other);
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	void fill(const unsigned int* data, size_t size, GLFLAG mode);
	void update(const unsigned int* data, size_t size, int offset);

	void bind() override;
	void unbind() override;
	void close() override;
};

};