#include "vectorMesh.h"
#include "../util/Log.h"

VectorMesh::VectorMesh(const double* vertices, const int vCount) : AbstractMesh(RenderMode::POINTS), vertexCount(vCount) {
	verticesBuffer = new VertexBuffer(vertices, vCount * 7);
	bufferLayout.push<double>(3);
	bufferLayout.push<double>(3);
	bufferLayout.push<double>(1);
	vertexArray->addBuffer(*verticesBuffer, bufferLayout);
}

void VectorMesh::render() {
	vertexArray->bind();
	glDrawArrays((int)renderMode, 0, vertexCount);
}

void VectorMesh::close() {
	verticesBuffer->close();
	vertexArray->close();
}