#pragma once

#include "../bindable.h"

class IndexBuffer : public Bindable {
public:
	IndexBuffer();
	IndexBuffer(const unsigned int* data, unsigned int size, unsigned int mode);
	IndexBuffer(const unsigned int* data, unsigned int size);
	~IndexBuffer();

	void bind() override;
	void unbind() override;
	void close() override;
};