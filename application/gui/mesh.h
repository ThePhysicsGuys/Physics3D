#pragma once
#include "../engine/geometry/shape.h"
#include <vector>

class Mesh {
public:
	enum class RenderMode {
		TRIANGLES = GL_TRIANGLES,
		LINES = GL_LINES,
		POINTS = GL_POINTS
	};

	unsigned int vao;
	unsigned int posVbo;
	unsigned int indVbo;

	const int vertexCount;
	const int triangleCount;
	RenderMode renderMode;

	void render();
	void close();
	Mesh(Shape shape);
	Mesh() : vertexCount(0), triangleCount(0) {}
};

