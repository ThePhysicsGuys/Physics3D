#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>

#include "mesh.h"

Mesh::Mesh(Shape shape) : vertexCount(shape.vertexCount), triangleCount(shape.triangleCount), renderMode(RenderMode::TRIANGLES) {
	// Mesh vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Position VBO
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCount, reinterpret_cast<double*>(shape.vertices), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	// Indices VBO
	glGenBuffers(1, &indVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleCount * 3, reinterpret_cast<int*>(shape.triangles), GL_STATIC_DRAW);

	// Reset
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::render() {
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	glDrawElements((int) renderMode, vertexCount, GL_UNSIGNED_INT, 0);

	glDisableVertexArrayAttrib(vao, 0);
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