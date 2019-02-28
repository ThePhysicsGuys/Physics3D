#include "indexedMesh.h"
#include "../../engine/geometry/shape.h"
#include "../util/log.h"

IndexedMesh::IndexedMesh(Shape shape) : AbstractMesh(), vertexCount(shape.vertexCount), triangleCount(shape.triangleCount) {
	vertexBuffer = new VertexBuffer(reinterpret_cast<double const *>(shape.vertices), vertexCount * 3);
	indexBuffer = new IndexBuffer(reinterpret_cast<unsigned int const *>(shape.triangles), triangleCount * 3);
	bufferLayout.push<double>(3);
	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

IndexedMesh::IndexedMesh(const double* vertices, const unsigned int* indices, const int vertexCount, const int tCount) : AbstractMesh(), vertexCount(vertexCount), triangleCount(tCount) {
	vertexBuffer = new VertexBuffer(vertices, vertexCount * 3);
	indexBuffer = new IndexBuffer(indices, triangleCount * 3);
	bufferLayout.push<double>(3);
	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

void IndexedMesh::render()  {
	vertexArray->bind();
	indexBuffer->bind();

	glDrawElements((int) renderMode, triangleCount * 3, GL_UNSIGNED_INT, nullptr);
}

void IndexedMesh::close() {
	vertexBuffer->close();
	indexBuffer->close();
	vertexArray->close();
}
