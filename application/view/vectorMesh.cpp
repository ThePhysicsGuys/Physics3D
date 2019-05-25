#include "vectorMesh.h"
#include "../util/Log.h"

VectorMesh::VectorMesh(const double* vertices, const size_t vertexCount, size_t capacity) : AbstractMesh(RenderMode::POINTS), vertexCount(vertexCount), capacity(capacity) {
	vertexBuffer = new VertexBuffer(vertices, capacity * 7, GL_DYNAMIC_DRAW);
	bufferLayout.push<double>(3);
	bufferLayout.push<double>(3);
	bufferLayout.push<double>(1);
	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

void VectorMesh::render() {
	vertexArray->bind();
	glDrawArrays((int) renderMode, 0, vertexCount);
}

void VectorMesh::close() {
	vertexBuffer->close();
	vertexArray->close();
}

void VectorMesh::update(const double* vertices, const size_t vertexCount) {
	vertexBuffer->bind();
	this->vertexCount = vertexCount;
	if (vertexCount > capacity) {
		capacity = vertexCount;
		Log::warn("Vector buffer overflow, creating new buffer with size (%d)", vertexCount);
		glBufferData(GL_ARRAY_BUFFER, capacity * 7 * sizeof(double), vertices, GL_DYNAMIC_DRAW);
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * 7 * sizeof(double), vertices);
	}
}