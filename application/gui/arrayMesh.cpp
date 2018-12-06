#include "arrayMesh.h"
#include "../util/Log.h"

ArrayMesh::ArrayMesh(const double* vertices, const int vCount) : AbstractMesh(), vertexCount(vCount) {
	vertexBuffer = new VertexBuffer(vertices, vCount * 3);
	bufferLayout.push<double>(3);
	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

void ArrayMesh::render() {
	vertexArray->bind();
	glDrawArrays((int) renderMode, 0, vertexCount);
}

void ArrayMesh::close() {
	vertexBuffer->close();
	vertexArray->close();
}