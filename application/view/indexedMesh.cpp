#include "indexedMesh.h"
#include "../../engine/geometry/shape.h"
#include "../util/log.h"

IndexedMesh::IndexedMesh(const Shape& shape) : AbstractMesh(), vertexCount(shape.vertexCount), triangleCount(shape.triangleCount) {
	double* vertices = new double[shape.vertexCount * 3];
	for(int i = 0; i < vertexCount; i++) {
		Vec3f vertex = shape[i];
		vertices[i * 3] = vertex.x;
		vertices[i * 3 + 1] = vertex.y;
		vertices[i * 3 + 2] = vertex.z;
	}

	vertexBuffer = new VertexBuffer(vertices, vertexCount * 3);
	normalBuffer = new VertexBuffer(reinterpret_cast<double const *>(shape.normals.get()), vertexCount * 3);
	uvBuffer = new VertexBuffer(reinterpret_cast<double const *>(shape.uvs.get()), vertexCount * 2);

	indexBuffer = new IndexBuffer(reinterpret_cast<unsigned int const *>(shape.triangles), triangleCount * 3);

	vertexBufferLayout.push<double>(3);
	normalBufferLayout.push<double>(3);
	uvBufferLayout.push<double>(2);

	vertexArray->addBuffer(*vertexBuffer, vertexBufferLayout);
	vertexArray->addBuffer(*normalBuffer, normalBufferLayout);
	vertexArray->addBuffer(*uvBuffer, uvBufferLayout);
}

IndexedMesh::IndexedMesh(const double* vertices, const double* normals, const double* uvs, const unsigned int* indices, const int vertexCount, const int triangleCount) : AbstractMesh(), vertexCount(vertexCount), triangleCount(triangleCount) {
	vertexBuffer = new VertexBuffer(vertices, vertexCount * 3);
	normalBuffer = new VertexBuffer(normals, vertexCount * 3);
	uvBuffer = new VertexBuffer(uvs, vertexCount * 2);

	indexBuffer = new IndexBuffer(indices, triangleCount * 3);

	vertexBufferLayout.push<double>(3);
	normalBufferLayout.push<double>(3);
	uvBufferLayout.push<double>(2);

	vertexArray->addBuffer(*vertexBuffer, vertexBufferLayout);
	vertexArray->addBuffer(*normalBuffer, normalBufferLayout);
	vertexArray->addBuffer(*uvBuffer, uvBufferLayout);
}

void IndexedMesh::render() {
	render(GL_FILL);
}

void IndexedMesh::render(int mode)  {
	vertexArray->bind();
	indexBuffer->bind();

	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glDrawElements((int) renderMode, triangleCount * 3, GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void IndexedMesh::close() {
	vertexBuffer->close();
	indexBuffer->close();
	vertexArray->close();
}
