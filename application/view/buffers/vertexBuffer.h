#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../bindable.h"

#include "../util/log.h"

class VertexBuffer : public Bindable {
public:
	VertexBuffer();
	VertexBuffer(const float* data, size_t size, unsigned int mode);
	VertexBuffer(const float* data, size_t size);
	~VertexBuffer();

	void bind() override;
	void unbind() override;
	void close() override;
};
