#pragma once

#include "../bindable.h"

namespace P3D::Graphics {

struct BufferLayout;
class VertexBuffer;

class VertexArray : public Bindable {
public:
	size_t attributeArrayOffset;

	VertexArray();

	~VertexArray();
	VertexArray(VertexArray&& other);
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(VertexArray&& other);
	VertexArray& operator=(const VertexArray&) = delete;

	void bind() override;
	void unbind() override;
	void close() override;

	void addBuffer(VertexBuffer* buffer);
};

};