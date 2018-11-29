#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

class AbstractMesh {
public:
	enum class RenderMode {
		TRIANGLES = GL_TRIANGLES,
		LINES = GL_LINES,
		POINTS = GL_POINTS
	};

	RenderMode renderMode;

	AbstractMesh() : renderMode(RenderMode::TRIANGLES) {};
	AbstractMesh(RenderMode rendermode) : renderMode(renderMode) {};

	virtual void render() = 0;
	virtual void close() = 0;
};

