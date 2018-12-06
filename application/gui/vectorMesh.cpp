#include "vectorMesh.h"
#include "../util/Log.h"

VectorMesh::VectorMesh(const double* positions, const double* rotations, const int vCount) : AbstractMesh(RenderMode::POINTS), vertexCount(vCount) {
	positionBuffer = new VertexBuffer(positions, vCount * 3);
	bufferLayout.push<double>(3);
	vertexArray->addBuffer(*positionBuffer, bufferLayout);

	rotationBuffer = new VertexBuffer(rotations, vCount * 3);
	bufferLayout.push<double>(3);
	vertexArray->addBuffer(*rotationBuffer, bufferLayout);
}

void VectorMesh::render() {
	vertexArray->bind();
	glDrawArrays((int)renderMode, 0, vertexCount);
}

void VectorMesh::close() {
	positionBuffer->close();
	rotationBuffer->close();
	vertexArray->close();
}