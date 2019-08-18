#pragma once

#include "../bindable.h"

#include "../util/log.h"

class VertexBuffer : public Bindable {
public:
	VertexBuffer();
	VertexBuffer(const void * data, size_t sizeInBytes, unsigned int mode);
	VertexBuffer(const void * data, size_t sizeInBytes);
	~VertexBuffer();

	void fill(const void * data, size_t sizeInBytes, unsigned int mode);
	void update(const void * data, size_t sizeInBytes, int offset);

	void bind() override;
	void unbind() override;
	void close() override;
};
