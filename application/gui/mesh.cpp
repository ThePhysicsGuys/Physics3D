#include "mesh.h"
#include "../util/Log.h"

void createPositionBuffer(unsigned int& vbo, int size, double* positions) {
	Log::debug("Generating position buffer");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(double), positions, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	Log::debug("Generated position buffer");
}

Mesh::Mesh(double* positions, int size) {
	createPositionBuffer(vbo, size, positions);
}

void Mesh::render() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays((int) renderMode, 0, 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::close() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vbo);
}