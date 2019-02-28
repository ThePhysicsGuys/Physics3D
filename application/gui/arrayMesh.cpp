#include "arrayMesh.h"
#include "../util/Log.h"

ArrayMesh::ArrayMesh(const double* vertices, const int vertexCount, const int dimensions, RenderMode renderMode) : AbstractMesh(renderMode), vertexCount(vertexCount) {
	vertexBuffer = new VertexBuffer(vertices, vertexCount * dimensions);
	bufferLayout.push<double>(dimensions);
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