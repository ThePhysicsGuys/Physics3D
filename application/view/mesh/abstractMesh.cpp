#include "abstractMesh.h"

#include "GL\glew.h"

#include "../buffers/vertexArray.h"

namespace RenderMode {
	int PATCHES = GL_PATCHES;
	int QUADS = GL_QUADS;
	int TRIANGLES = GL_TRIANGLES;
	int LINES = GL_LINES;
	int POINTS = GL_POINTS;
}

AbstractMesh::AbstractMesh(int rendermode) : renderMode(renderMode) {
	vertexArray = new VertexArray();
};

AbstractMesh::AbstractMesh() : renderMode(RenderMode::TRIANGLES) {
	vertexArray = new VertexArray();
};