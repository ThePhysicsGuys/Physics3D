#include "vectorMesh.h"

#include "../util/Log.h"

#include "../renderUtils.h"

#include "../buffers/vertexBuffer.h"
#include "../buffers/vertexArray.h"

VectorMesh::VectorMesh(const float* vertices, const size_t vertexCount, size_t capacity) : AbstractMesh(Renderer::POINT), vertexCount(vertexCount), capacity(capacity) {
	vertexBuffer = new VertexBuffer(vertices, 9 * capacity * sizeof(float), Renderer::DYNAMIC_DRAW);
	
	bufferLayout = {
		{
			{ "vposition", BufferDataType::FLOAT3 },
			{ "vdirection", BufferDataType::FLOAT3 },
			{ "vcolor", BufferDataType::FLOAT3 }
		}
	};

	vertexArray->addBuffer(*vertexBuffer, bufferLayout);
}

void VectorMesh::render() {
	vertexArray->bind();
	Renderer::drawArrays(renderMode, 0, vertexCount);
}

void VectorMesh::close() {
	vertexBuffer->close();
	vertexArray->close();
}

void VectorMesh::update(const float* vertices, const size_t vertexCount) {
	vertexBuffer->bind();
	this->vertexCount = vertexCount;
	if (vertexCount > capacity) {
		capacity = vertexCount;
		Log::warn("Vector buffer overflow, creating new buffer with size (%d)", vertexCount);
		vertexBuffer->fill(vertices, capacity * bufferLayout.stride * sizeof(float), Renderer::DYNAMIC_DRAW);
	} else {
		vertexBuffer->update(vertices, vertexCount * bufferLayout.stride * sizeof(float), 0);
	}
}