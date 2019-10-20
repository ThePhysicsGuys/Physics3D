#include "core.h"

#include "abstractMesh.h"

#include "buffers/vertexArray.h"
#include "renderUtils.h"

AbstractMesh::AbstractMesh(unsigned int rendermode) : renderMode(renderMode) {
	vertexArray = new VertexArray();
};

AbstractMesh::AbstractMesh() : renderMode(Renderer::TRIANGLES) {
	vertexArray = new VertexArray();
};