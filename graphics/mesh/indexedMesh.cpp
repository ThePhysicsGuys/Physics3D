#include "core.h"

#include "indexedMesh.h"

#include "buffers/indexBuffer.h"
#include "buffers/vertexBuffer.h"
#include "buffers/vertexArray.h"
#include "visualShape.h"
#include "renderUtils.h"
#include "../physics/geometry/shape.h"

IndexedMesh::IndexedMesh(const VisualShape& shape) : AbstractMesh(), vertexCount(shape.vertexCount), triangleCount(shape.triangleCount) {
	float* vertices = new float[shape.vertexCount * 3];
	for(int i = 0; i < vertexCount; i++) {
		Vec3f vertex = shape[i];
		vertices[i * 3] = vertex.x;
		vertices[i * 3 + 1] = vertex.y;
		vertices[i * 3 + 2] = vertex.z;
	}
	unsigned int* triangles = new unsigned int[shape.triangleCount * 3];
	for (int i = 0; i < triangleCount; i++) {
		Triangle triangle = shape.getTriangle(i);
		triangles[i * 3] = triangle.firstIndex;
		triangles[i * 3 + 1] = triangle.secondIndex;
		triangles[i * 3 + 2] = triangle.thirdIndex;
	}

	vertexBuffer = new VertexBuffer(vertices, 3 * vertexCount * sizeof(float));
	normalBuffer = new VertexBuffer(reinterpret_cast<float const *>(shape.normals.get()), 3 * vertexCount * sizeof(float));
	uvBuffer = new VertexBuffer(reinterpret_cast<float const *>(shape.uvs.get()), 2 * vertexCount * sizeof(float));

	indexBuffer = new IndexBuffer(triangles, 3 * triangleCount);

	vertexBufferLayout = {
		{{ "vposition", BufferDataType::FLOAT3 }}
	};

	normalBufferLayout = {
		{{ "vnormal", BufferDataType::FLOAT3 }}
	};

	uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};

	vertexArray->addBuffer(*vertexBuffer, vertexBufferLayout);
	vertexArray->addBuffer(*normalBuffer, normalBufferLayout);
	vertexArray->addBuffer(*uvBuffer, uvBufferLayout);
}

IndexedMesh::IndexedMesh(const float* vertices, const float* normals, const float* uvs, const unsigned int* indices, const int vertexCount, const int triangleCount) : AbstractMesh(), vertexCount(vertexCount), triangleCount(triangleCount) {
	vertexBuffer = new VertexBuffer(vertices, 3 * vertexCount * sizeof(float));
	normalBuffer = new VertexBuffer(normals, 3 * vertexCount * sizeof(float));
	uvBuffer = new VertexBuffer(uvs, 2 * vertexCount * sizeof(float));

	indexBuffer = new IndexBuffer(indices, 3 * triangleCount);

	vertexBufferLayout = {
		{{ "vposition", BufferDataType::FLOAT3 }}
	};

	normalBufferLayout = {
		{{ "vnormal", BufferDataType::FLOAT3 }}
	};

	uvBufferLayout = {
		{{ "vUV", BufferDataType::FLOAT2 }}
	};


	vertexArray->addBuffer(*vertexBuffer, vertexBufferLayout);
	vertexArray->addBuffer(*normalBuffer, normalBufferLayout);
	vertexArray->addBuffer(*uvBuffer, uvBufferLayout);
}

void IndexedMesh::render() {
	render(Renderer::FILL);
}

void IndexedMesh::render(unsigned int mode)  {
	vertexArray->bind();
	indexBuffer->bind();

	glPolygonMode(GL_FRONT_AND_BACK, mode);
	Renderer::drawElements(renderMode, triangleCount * 3, Renderer::UINT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void IndexedMesh::close() {
	vertexBuffer->close();
	indexBuffer->close();
	vertexArray->close();
}
