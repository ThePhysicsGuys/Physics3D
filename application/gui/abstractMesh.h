#pragma once

#include "GL\glew.h"
#include "vertexArray.h"
#include "../../util/log.h"

class AbstractMesh {
public:
	enum class RenderMode {
		TRIANGLES = GL_TRIANGLES,
		LINES = GL_LINES,
		POINTS = GL_POINTS
	};

	VertexArray* vertexArray = nullptr;
	RenderMode renderMode;

	AbstractMesh(RenderMode rendermode) : renderMode(renderMode) {
		Log::debug("Created new AbstractMesh with renderMode %d", (int) renderMode);
		vertexArray = new VertexArray();
	};

	AbstractMesh() : renderMode(RenderMode::TRIANGLES) {
		Log::debug("Created new AbstractMesh with renderMode %d", (int)renderMode);
		vertexArray = new VertexArray();
	};

	virtual void render() = 0;
	virtual void close() = 0;
};

