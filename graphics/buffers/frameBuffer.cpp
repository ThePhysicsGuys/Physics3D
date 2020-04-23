#include "core.h"

#include "frameBuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "renderBuffer.h"
#include "texture.h"

namespace Graphics {

#pragma region FrameBuffer

//! FrameBuffer

FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &id);
	bind();
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) : FrameBuffer() {
	texture = new Texture(width, height);
	renderBuffer = new RenderBuffer(width, height);

	attach(texture);
	attach(renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
}

FrameBuffer::FrameBuffer(Texture* colorAttachment, RenderBuffer* depthStencilAttachment) : FrameBuffer() {
	attach(colorAttachment);
	attach(depthStencilAttachment);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
}

FrameBuffer::~FrameBuffer() {
	close();
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) {
	id = other.id;
	other.id = 0;

	texture = other.texture;
	other.texture = nullptr;

	renderBuffer = other.renderBuffer;
	other.renderBuffer = nullptr;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
		std::swap(texture, other.texture);
		std::swap(renderBuffer, other.renderBuffer);
	}

	return *this;
}

void FrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
}

void FrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
}

void FrameBuffer::resize(Vec2i dimension) {
	this->dimension = dimension;

	if (texture)
		texture->resize(dimension.x, dimension.y);

	if (renderBuffer)
		renderBuffer->resize(dimension.x, dimension.y);
}

void FrameBuffer::attach(Texture* texture) {
	bind();
	this->texture = texture;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);
}

void FrameBuffer::attach(RenderBuffer* renderBuffer) {
	bind();
	this->renderBuffer = renderBuffer;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());
}

void FrameBuffer::close() {
	Log::warn("Closing framebuffer #%d", id);

	if (texture)
		texture->close();

	if (renderBuffer)
		renderBuffer->close();

	glDeleteFramebuffers(1, &id);

	texture = nullptr;
	renderBuffer = nullptr;
	id = 0;
}

#pragma endregion

#pragma region HDRFrameBuffer

//! HDRFrameBuffer

HDRFrameBuffer::HDRFrameBuffer() {
	glGenFramebuffers(1, &id);
	bind();
}

HDRFrameBuffer::HDRFrameBuffer(unsigned int width, unsigned int height) : HDRFrameBuffer() {
	texture = new HDRTexture(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);

	renderBuffer = new RenderBuffer(width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
};

HDRFrameBuffer::~HDRFrameBuffer() {
	close();
}

HDRFrameBuffer::HDRFrameBuffer(HDRFrameBuffer&& other) {
	id = other.id;
	other.id = 0;

	texture = other.texture;
	other.texture = nullptr;

	renderBuffer = other.renderBuffer;
	other.renderBuffer = nullptr;
}

HDRFrameBuffer& HDRFrameBuffer::operator=(HDRFrameBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
		std::swap(texture, other.texture);
		std::swap(renderBuffer, other.renderBuffer);
	}

	return *this;
}

void HDRFrameBuffer::resize(Vec2i dimension) {
	if (texture)
		texture->resize(dimension.x, dimension.y);
	if (renderBuffer)
		renderBuffer->resize(dimension.x, dimension.y);
};

void HDRFrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
};

void HDRFrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
};

void HDRFrameBuffer::close() {
	Log::warn("Closing HDR framebuffer #%d", id);

	texture->close();
	renderBuffer->close();

	glDeleteFramebuffers(1, &id);
	id = 0;
};

#pragma endregion

#pragma region MultisampleFrameBuffer

//! MultisampleFrameBuffer

MultisampleFrameBuffer::MultisampleFrameBuffer() {
	glGenFramebuffers(1, &id);
	bind();
}

MultisampleFrameBuffer::MultisampleFrameBuffer(unsigned int width, unsigned int height, int samples) : MultisampleFrameBuffer() {
	texture = new MultisampleTexture(width, height, samples);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture->getID(), 0);

	renderBuffer = new MultisampleRenderBuffer(width, height, samples);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
};

MultisampleFrameBuffer::~MultisampleFrameBuffer() {
	close();
}

MultisampleFrameBuffer::MultisampleFrameBuffer(MultisampleFrameBuffer&& other) {
	id = other.id;
	other.id = 0;

	texture = other.texture;
	other.texture = nullptr;

	renderBuffer = other.renderBuffer;
	other.renderBuffer = nullptr;
}

MultisampleFrameBuffer& MultisampleFrameBuffer::operator=(MultisampleFrameBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
		std::swap(texture, other.texture);
		std::swap(renderBuffer, other.renderBuffer);
	}

	return *this;
}

void MultisampleFrameBuffer::resize(Vec2i dimension) {
	if (texture)
		texture->resize(dimension.x, dimension.y);
	if (renderBuffer)
		renderBuffer->resize(dimension.x, dimension.y);
};

void MultisampleFrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
};

void MultisampleFrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
};

void MultisampleFrameBuffer::close() {
	Log::warn("Closing multisample framebuffer #%d", id);

	texture->close();
	renderBuffer->close();

	glDeleteFramebuffers(1, &id);
	id = 0;
};

#pragma endregion

#pragma region DepthFrameBuffer

//! DepthFrameBuffer

DepthFrameBuffer::DepthFrameBuffer(unsigned int width, unsigned int height) {
	texture = new DepthTexture(width, height);
	glGenFramebuffers(1, &id);
	Renderer::bindFramebuffer(id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	Renderer::bindFramebuffer(0);
}

DepthFrameBuffer::~DepthFrameBuffer() {
	close();
}

DepthFrameBuffer::DepthFrameBuffer(DepthFrameBuffer&& other) {
	id = other.id;
	other.id = 0;

	texture = other.texture;
	other.texture = nullptr;
}

DepthFrameBuffer& DepthFrameBuffer::operator=(DepthFrameBuffer&& other) {
	if (this != &other) {
		close();
		std::swap(id, other.id);
		std::swap(texture, other.texture);
	}

	return *this;
}

void DepthFrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
}

void DepthFrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
}

void DepthFrameBuffer::close() {
	Log::warn("Closing depth framebuffer #%d", id);

	texture->close();
	glDeleteFramebuffers(1, &id);
	id = 0;
}

#pragma endregion

}