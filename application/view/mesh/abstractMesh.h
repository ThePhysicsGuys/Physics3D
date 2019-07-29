#pragma once

#include "../buffers/bufferLayout.h"
#include "../util/log.h"

namespace RenderMode {
	extern int PATCHES;
	extern int QUADS;
	extern int TRIANGLES;
	extern int LINES;
	extern int POINTS;
};

class VertexArray;

class AbstractMesh {
public:
	VertexArray* vertexArray = nullptr;
	BufferLayout bufferLayout;
	int renderMode;

	AbstractMesh(int rendermode);
	AbstractMesh();

	virtual void render() = 0;
	virtual void close() = 0;
};

