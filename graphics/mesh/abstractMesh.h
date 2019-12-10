#pragma once

#include "../buffers/bufferLayout.h"

class VertexArray;

class AbstractMesh {
public:
	VertexArray* vao = nullptr;
	BufferLayout bufferLayout;
	unsigned int renderMode;

	AbstractMesh(unsigned int rendermode);
	AbstractMesh();

	virtual void render() = 0;
	virtual void close() = 0;
};

