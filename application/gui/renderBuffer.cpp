#include "renderBuffer.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

RenderBuffer::RenderBuffer(unsigned int width, unsigned int height) : width(width), height(height) {
	glGenRenderbuffers(1, &id);

	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	unbind();
}

void RenderBuffer::resize(unsigned int width, unsigned int height) {
	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	unbind();
}

void RenderBuffer::bind() {
	glBindRenderbuffer(GL_RENDERBUFFER, id);
}

void RenderBuffer::unbind() {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::close() {
	glDeleteRenderbuffers(1, &id);
}