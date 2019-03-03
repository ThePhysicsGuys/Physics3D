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

TextureMultisample::TextureMultisample(unsigned int width, unsigned int height, unsigned int samples) : width(width), height(height), samples(samples) {
	glGenTextures(1, &id);
	bind();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unbind();
}

void TextureMultisample::resize(unsigned int width, unsigned int height) {
	bind();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	unbind();
}

void TextureMultisample::bind() {
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
}

void TextureMultisample::unbind() {
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void TextureMultisample::close() {
	glDeleteTextures(1, &id);
}