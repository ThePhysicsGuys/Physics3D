#include "core.h"

#include "vectorMesh.h"

#include "renderer.h"

#include "buffers/vertexBuffer.h"
#include "buffers/vertexArray.h"

namespace Graphics {

VectorMesh::VectorMesh(const float* vertices, const size_t vertexCount, size_t capacity) : AbstractMesh(Graphics::Renderer::POINT), vertexCount(vertexCount), capacity(capacity) {
	vertexBuffer = new VertexBuffer(vertices, 9 * capacity * sizeof(float), Graphics::Renderer::DYNAMIC_DRAW);

	vertexBufferLayout = BufferLayout({
		BufferElement("vposition", BufferDataType::FLOAT3),
		BufferElement("vdirection", BufferDataType::FLOAT3),
		BufferElement("vcolor", BufferDataType::FLOAT3)
	});

	vao->addBuffer(vertexBuffer, vertexBufferLayout);
}

void VectorMesh::render() {
	vao->bind();
	Graphics::Renderer::drawArrays(renderMode, 0, vertexCount);
}

void VectorMesh::close() {
	vertexBuffer->close();
	vao->close();
}

void VectorMesh::update(const float* vertices, const size_t vertexCount) {
	vertexBuffer->bind();
	this->vertexCount = vertexCount;
	if (vertexCount > capacity) {
		capacity = vertexCount;
		Log::warn("Vector buffer overflow, creating new buffer with size (%d)", vertexCount);
		vertexBuffer->fill(vertices, capacity * vertexBufferLayout.stride * sizeof(float), Graphics::Renderer::DYNAMIC_DRAW);
	} else {
		vertexBuffer->update(vertices, vertexCount * vertexBufferLayout.stride * sizeof(float), 0);
	}
}

};