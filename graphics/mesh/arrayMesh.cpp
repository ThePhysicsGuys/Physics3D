#include "core.h"

#include "arrayMesh.h"

#include "buffers/vertexArray.h"
#include "buffers/vertexBuffer.h"

#include "renderer.h"

namespace P3D::Graphics {

ArrayMesh::ArrayMesh(Vec3f* positions, const unsigned int vertexCount) : ArrayMesh(reinterpret_cast<float const*>(positions), vertexCount * 3, 3) {

};

ArrayMesh::ArrayMesh(Vec2f* positions, const unsigned int vertexCount) : ArrayMesh(reinterpret_cast<float const*>(positions), vertexCount * 2, 2) {

};

ArrayMesh::ArrayMesh(const float* positions, const unsigned int vertexCount, const unsigned int dimensions) : ArrayMesh(positions, vertexCount, dimensions, Graphics::Renderer::TRIANGLES) {

};

ArrayMesh::ArrayMesh(const float* vertices, const float* uv, const unsigned int vertexCount, const unsigned int dimensions) : AbstractMesh(), vertexCount(vertexCount) {
	vertexBufferLayout = {
		{{ "vposition", (dimensions == 2) ? BufferDataType::FLOAT2 : BufferDataType::FLOAT3 }}
	};
	vertexBuffer = new VertexBuffer(vertexBufferLayout, vertices, dimensions * vertexCount * sizeof(float));

	uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};
	uvBuffer = new VertexBuffer(uvBufferLayout, uv, 2 * vertexCount * sizeof(float));

	vao->addBuffer(vertexBuffer);
	vao->addBuffer(uvBuffer);
}

ArrayMesh::ArrayMesh(const float* vertices, const unsigned int vertexCount, const unsigned int dimensions, unsigned int renderMode) : AbstractMesh(renderMode), vertexCount(vertexCount) {
	vertexBufferLayout = {
		{{ "vposition", (dimensions == 2) ? BufferDataType::FLOAT2 : BufferDataType::FLOAT3 }}
	};
	vertexBuffer = new VertexBuffer(vertexBufferLayout, vertices, dimensions * vertexCount * sizeof(float));

	vao->addBuffer(vertexBuffer);
}

void ArrayMesh::render() {
	vao->bind();
	Graphics::Renderer::drawArrays(renderMode, 0, vertexCount);
}

void ArrayMesh::close() {
	vertexBuffer->close();
	vao->close();
}

};