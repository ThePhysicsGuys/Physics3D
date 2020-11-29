#include "core.h"

#include "pointMesh.h"

#include "buffers/vertexArray.h"
#include "buffers/vertexBuffer.h"
#include "renderer.h"

namespace P3D::Graphics {

PointMesh::PointMesh(const float* vertices, const size_t vertexCount, size_t capacity) : AbstractMesh(Renderer::POINT), vertexCount(vertexCount), capacity(capacity) {
	vertexBufferLayout = BufferLayout({
		BufferElement("vposition", BufferDataType::FLOAT3),
		BufferElement("vsize", BufferDataType::FLOAT),
		BufferElement("vcolor1", BufferDataType::FLOAT3),
		BufferElement("vcolor2", BufferDataType::FLOAT3)
	});

	vertexBuffer = new VertexBuffer(vertexBufferLayout, nullptr, 10 * capacity * sizeof(float), Renderer::DYNAMIC_DRAW);
	
	vao->addBuffer(vertexBuffer);
}

void PointMesh::render() {
	vao->bind();
	Renderer::drawArrays(renderMode, 0, vertexCount);
}

void PointMesh::close() {
	vertexBuffer->close();
	vao->close();
}

void PointMesh::update(const float* vertices, const size_t vertexCount) {
	vertexBuffer->bind();
	this->vertexCount = vertexCount;
	if (vertexCount > capacity) {
		capacity = vertexCount;
		Log::warn("Point buffer overflow, creating new buffer with size (%d)", vertexCount);
		vertexBuffer->fill(vertices, capacity * vertexBufferLayout.stride * sizeof(float), Graphics::Renderer::DYNAMIC_DRAW);
	} else {
		vertexBuffer->update(vertices, vertexCount * vertexBufferLayout.stride * sizeof(float), 0);
	}
}

};