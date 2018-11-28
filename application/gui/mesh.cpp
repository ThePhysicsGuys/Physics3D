#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>

#include "mesh.h"

#include <vector>

int* createElementBuffer(Triangle* triangles, const int triangleCount) {
	std::vector<int> indices;
	for (int i = 0; i < triangleCount; i++) {
		indices.push_back(triangleCount + i);
	}
	return indices.data();
}

Mesh::Mesh(Shape shape) : vertexCount(shape.vertexCount), triangleCount(shape.triangleCount), renderMode(RenderMode::TRIANGLES) {
	// Mesh vao
	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Position VBO
	posVbo = 0;
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount, reinterpret_cast<double*>(shape.vertices), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	// Indices VBO
	indVbo = 0;
	glGenBuffers(1, &indVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleCount * 3, createElementBuffer(shape.triangles, triangleCount), GL_STATIC_DRAW);

	// Reset
	glBindVertexArray(0);
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