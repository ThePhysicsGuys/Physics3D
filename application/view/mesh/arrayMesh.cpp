#include "arrayMesh.h"

#include "../buffers/vertexArray.h"
#include "../buffers/vertexBuffer.h"

ArrayMesh::ArrayMesh(const float* vertices, const float* uv, const unsigned int vertexCount, const unsigned int dimensions) : AbstractMesh(), vertexCount(vertexCount) {
	vertexBuffer = new VertexBuffer(vertices, vertexCount * dimensions);
	uvBuffer = new VertexBuffer(uv, vertexCount * 2);

	vertexBufferLayout = { 
		{{ "vposition", (dimensions == 2) ? BufferDataType::FLOAT2 : BufferDataType::FLOAT3 }} 
	};

	uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};

	vertexArray->addBuffer(*vertexBuffer, vertexBufferLayout);
	vertexArray->addBuffer(*uvBuffer, uvBufferLayout);
}

ArrayMesh::ArrayMesh(const float* vertices, const unsigned int vertexCount, const unsigned int dimensions, int renderMode) : AbstractMesh(renderMode), vertexCount(vertexCount) {
	vertexBuffer = new VertexBuffer(vertices, vertexCount * dimensions);

	vertexBufferLayout = {
		{{ "vposition", (dimensions == 2) ? BufferDataType::FLOAT2 : BufferDataType::FLOAT3 }}
	};

	vertexArray->addBuffer(*vertexBuffer, vertexBufferLayout);
}

void ArrayMesh::render() {
	vertexArray->bind();
	glDrawArrays((int) renderMode, 0, vertexCount);
}

void ArrayMesh::close() {
	vertexBuffer->close();
	vertexArray->close();
}