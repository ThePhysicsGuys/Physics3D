#include "core.h"

#include "shaderProgram.h"

void ShaderProgram::bind() {
	shader.bind();
}

void ShaderProgram::close() {
	shader.close();
}

ShaderProgram::~ShaderProgram() {
	close();
}