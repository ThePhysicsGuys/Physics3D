#pragma once

#include "../bindable.h"

class VertexBuffer;
struct BufferLayout;

class VertexArray : public Bindable {
public:
	size_t attributeArrayOffset;

	VertexArray();
	~VertexArray();

	void bind() override;
	void unbind() override;
	void close() override;

	void addBuffer(VertexBuffer& buffer, const BufferLayout& layout);
};