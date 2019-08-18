#pragma once

#include "../buffers/bufferLayout.h"
#include "../util/log.h"

class VertexArray;

class AbstractMesh {
public:
	VertexArray* vertexArray = nullptr;
	BufferLayout bufferLayout;
	unsigned int renderMode;

	AbstractMesh(unsigned int rendermode);
	AbstractMesh();

	virtual void render() = 0;
	virtual void close() = 0;
};

