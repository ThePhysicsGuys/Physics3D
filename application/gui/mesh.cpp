#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>

#include "mesh.h"
#include "../util/Log.h";

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

void createPositionBuffer(unsigned int& vbo, int size, double* buffer) {
	Log::debug("Generating position vbo");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);
	Log::debug("Generated position vbo");
}

void createElementBuffer(unsigned int& vbo, int size, int* buffer) {
	Log::debug("Generating index vbo");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
	Log::debug("Generated index vbo");
}

Mesh::Mesh(Shape shape) : vertexCount(shape.vertexCount), triangleCount(shape.triangleCount), renderMode(RenderMode::TRIANGLES) {
	// Mesh vao
	createVertexArray(vao);

	// Position VBO
	createPositionBuffer(posVbo, vertexCount, reinterpret_cast<double*>(shape.vertices));

	// Indices VBO
	createElementBuffer(indVbo, triangleCount * 3, reinterpret_cast<int*>(shape.triangles));
}

Mesh::Mesh(Vec3* vertices, int vertexCount, Triangle* triangles, int triangleCount) : vertexCount(vertexCount), triangleCount(triangleCount) , renderMode(RenderMode::TRIANGLES) {
	// Mesh vao
	createVertexArray(vao);

	// Position VBO
	createPositionBuffer(posVbo, vertexCount, reinterpret_cast<double*>(vertices));

	// Indices VBO
	createElementBuffer(indVbo, triangleCount * 3, reinterpret_cast<int*>(triangles));
}

void Mesh::render() {
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	glDrawElements((int) renderMode, vertexCount, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Mesh::close() {
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(vertexCount, &posVbo);
	glDeleteBuffers(triangleCount * 3, &indVbo);
	glDeleteVertexArrays(1, &vao);
}