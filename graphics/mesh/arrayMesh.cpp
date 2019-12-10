#include "core.h"

#include "arrayMesh.h"

#include "buffers/vertexArray.h"
#include "buffers/vertexBuffer.h"

#include "renderUtils.h"

ArrayMesh::ArrayMesh(Vec3f* positions, const unsigned int vertexCount) : ArrayMesh(reinterpret_cast<float const *>(positions), vertexCount * 3, 3) {

};

ArrayMesh::ArrayMesh(Vec2f* positions, const unsigned int vertexCount) : ArrayMesh(reinterpret_cast<float const *>(positions), vertexCount * 2, 2) {

};

ArrayMesh::ArrayMesh(const float* positions, const unsigned int vertexCount, const unsigned int dimensions) : ArrayMesh(positions, vertexCount, dimensions, Renderer::TRIANGLES) {

};

ArrayMesh::ArrayMesh(const float* vertices, const float* uv, const unsigned int vertexCount, const unsigned int dimensions) : AbstractMesh(), vertexCount(vertexCount) {
	vertexBuffer = new VertexBuffer(vertices, dimensions * vertexCount * sizeof(float));
	uvBuffer = new VertexBuffer(uv, 2 * vertexCount * sizeof(float));

	vertexBufferLayout = { 
		{{ "vposition", (dimensions == 2) ? BufferDataType::FLOAT2 : BufferDataType::FLOAT3 }} 
	};

	uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};

	vao->addBuffer(vertexBuffer, vertexBufferLayout);
	vao->addBuffer(uvBuffer, uvBufferLayout);
}

ArrayMesh::ArrayMesh(const float* vertices, const unsigned int vertexCount, const unsigned int dimensions, unsigned int renderMode) : AbstractMesh(renderMode), vertexCount(vertexCount) {
	vertexBuffer = new VertexBuffer(vertices, dimensions * vertexCount * sizeof(float));

	vertexBufferLayout = {
		{{ "vposition", (dimensions == 2) ? BufferDataType::FLOAT2 : BufferDataType::FLOAT3 }}
	};

	vao->addBuffer(vertexBuffer, vertexBufferLayout);
}

void ArrayMesh::render() {
	vao->bind();
	Renderer::drawArrays(renderMode, 0, vertexCount);
}

void ArrayMesh::close() {
	vertexBuffer->close();
	vao->close();
}