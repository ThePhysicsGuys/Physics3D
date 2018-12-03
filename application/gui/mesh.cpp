#include "mesh.h"
#include "../util/Log.h"

Mesh::Mesh(const double* vertices, const int vCount) : vertexCount(vCount) {
	//createPositionBuffer(vbo, size, positions);
	vertexBuffer = new VertexBuffer(vertices, vCount * 3);
	bufferLayout.push<double>(3);
	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

void Mesh::render() {
	vertexArray->bind();
	glDrawArrays((int) renderMode, 0, vertexCount);
}

void Mesh::close() {
	vertexBuffer->close();
	vertexArray->close();
}