#include "indexedMesh.h"
#include "../../engine/geometry/shape.h"
#include "../util/log.h"

#include <vector>

int* createElementBufferData(Triangle triangles[], int triangleCount) {
	Log::debug("Creating element buffer");
	std::vector<int> indices;
	Log::debug("Reserving %d integers", triangleCount * 3);
	indices.reserve(triangleCount * 3);
	Log::debug("Reserved %d integers", triangleCount * 3);
	for (int i = 0; i < triangleCount; i++) {
		Log::debug("%d", i);
		indices.push_back(triangles[i].firstIndex);
		indices.push_back(triangles[i].secondIndex);
		indices.push_back(triangles[i].thirdIndex);
	}
	Log::debug("Created element buffer");
	return indices.data();
}

void createVertexArray(unsigned int &vao) {
	Log::debug("Generating vao");
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	Log::debug("Generated vao");

}

void createPositionBufferTest(unsigned int& vbo, int size, double const * buffer) {
	Log::debug("Generating position vbo");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(double), buffer, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), 0);
	Log::debug("Generated position vbo");
}

void createElementBuffer(unsigned int& vbo, int size, unsigned int const * buffer) {
	Log::debug("Generating index vbo");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(unsigned int), buffer, GL_STATIC_DRAW);
	Log::debug("Generated index vbo");
}

IndexedMesh::IndexedMesh(Shape shape) : vertexCount(shape.vertexCount()), triangleCount(shape.triangleCount()) {
	// Mesh vao
	createVertexArray(vao);

	// Position VBO
	createPositionBufferTest(posVbo, vertexCount * 3, reinterpret_cast<double const *>(shape.getVertices()));

	// Indices VBO
	createElementBuffer(indVbo, triangleCount * 3, reinterpret_cast<unsigned int const *>(shape.getTriangles()));
}

IndexedMesh::IndexedMesh(double* vertices, int vertexCount, unsigned int* triangles, int triangleCount) : vertexCount(vertexCount), triangleCount(triangleCount) {
	// Mesh vao
	createVertexArray(vao);

	// Position VBO
	createPositionBufferTest(posVbo, vertexCount * 3, vertices);

	// Indices VBO
	createElementBuffer(indVbo, triangleCount * 3, triangles);
}

void IndexedMesh::render()  {
	Log::debug("Rendered from indexedMesh.cpp");
	glDisable(GL_CULL_FACE);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	glDrawElements((int)renderMode, triangleCount * 3, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void IndexedMesh::close() {
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(vertexCount, &posVbo);
	glDeleteBuffers(triangleCount * 3, &indVbo);
	glDeleteVertexArrays(1, &vao);
}
