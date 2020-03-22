#pragma once

#include "../buffers/bufferLayout.h"
#include "../renderer.h"

namespace Graphics {

class VertexArray;

class AbstractMesh {
public:
	VertexArray* vao = nullptr;
	GLFLAG renderMode;

	AbstractMesh(GLFLAG renderMode);
	AbstractMesh();

	virtual void render() = 0;
	virtual void close() = 0;
};

};