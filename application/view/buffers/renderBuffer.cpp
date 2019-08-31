#include "renderBuffer.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include <utility>

RenderBuffer::RenderBuffer(unsigned int width, unsigned int height) : width(width), height(height) {
	glGenRenderbuffers(1, &id);

	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	unbind();
}

RenderBuffer::~RenderBuffer() {
	close();
}

RenderBuffer::RenderBuffer(RenderBuffer&& other) {
	id = other.id;
	other.id = 0;
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
	}
}


void RenderBuffer::resize(unsigned int width, unsigned int height) {
	bind();
	this->width = width;
	this->height = height;
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
	id = 0;
}


// MultisampleRenderBuffer

MultisampleRenderBuffer::MultisampleRenderBuffer(unsigned int width, unsigned int height, unsigned int samples) : width(width), height(height), samples(samples) {
	glGenRenderbuffers(1, &id);

	bind();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	unbind();
}

MultisampleRenderBuffer::~MultisampleRenderBuffer() {
	close();
}

MultisampleRenderBuffer::MultisampleRenderBuffer(MultisampleRenderBuffer&& other) {
	id = other.id;
	other.id = 0;
}

MultisampleRenderBuffer& MultisampleRenderBuffer::operator=(MultisampleRenderBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
	}
}

void MultisampleRenderBuffer::resize(unsigned int width, unsigned int height) {
	bind();
	this->width = width;
	this->height = height;
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	unbind();
}

void MultisampleRenderBuffer::bind() {
	glBindRenderbuffer(GL_RENDERBUFFER, id);
}

void MultisampleRenderBuffer::unbind() {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void MultisampleRenderBuffer::close() {
	glDeleteRenderbuffers(1, &id);
	id = 0;
}