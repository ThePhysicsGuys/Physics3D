#pragma once

#include "../bindable.h"

class IndexBuffer : public Bindable {
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, size_t size, unsigned int mode);
	IndexBuffer(const unsigned int* data, size_t size);
	~IndexBuffer();

	void fill(const unsigned int* data, size_t size, unsigned int mode);
	void update(const unsigned int* data, size_t size, int offset);

	void bind() override;
	void unbind() override;
	void close() override;
};