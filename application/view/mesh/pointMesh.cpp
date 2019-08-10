#include "pointMesh.h"

#include "../buffers/vertexArray.h"
#include "../buffers/vertexBuffer.h"

#include "../util/Log.h"

PointMesh::PointMesh(const float* vertices, const size_t vertexCount, size_t capacity) : AbstractMesh(RenderMode::POINTS), vertexCount(vertexCount), capacity(capacity) {
	vertexBuffer = new VertexBuffer(vertices, capacity * 10, GL_DYNAMIC_DRAW);

	bufferLayout = {
		{
			{ "vposition", BufferDataType::FLOAT3 },
			{ "vsize", BufferDataType::FLOAT },
			{ "vcolor1", BufferDataType::FLOAT3 },
			{ "vcolor2", BufferDataType::FLOAT3 }
		}
	};

	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

void PointMesh::render() {
	vertexArray->bind();
	glDrawArrays((int)renderMode, 0, vertexCount);
}

void PointMesh::close() {
	vertexBuffer->close();
	vertexArray->close();
}

void PointMesh::update(const float* vertices, const size_t vertexCount) {
	vertexBuffer->bind();
	this->vertexCount = vertexCount;
	if (vertexCount > capacity) {
		capacity = vertexCount;
		Log::warn("Point buffer overflow, creating new buffer with size (%d)", vertexCount);
		glBufferData(GL_ARRAY_BUFFER, capacity * bufferLayout.stride, vertices, GL_DYNAMIC_DRAW);
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * bufferLayout.stride, vertices);
	}
}
