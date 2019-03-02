#include "texture.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

Texture::Texture(unsigned int width, unsigned int height) : width(width), height(height) {
	glGenTextures(1, &id);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unbind();
}

void Texture::resize(unsigned int width, unsigned int height) {
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	unbind();
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::close() {
	glDeleteTextures(1, &id);
}