#include "core.h"

#include "frameBuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "renderBuffer.h"
#include "texture.h"

namespace P3D::Graphics {

#pragma region FrameBuffer

//! FrameBuffer
	
FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &id);
	bind();
	
	texture = std::make_shared<Texture>(width, height);
	renderBuffer = std::make_shared<RenderBuffer>(width, height);

	attach(texture);
	attach(renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
}

FrameBuffer::FrameBuffer(SRef<Texture> colorAttachment, SRef<RenderBuffer> depthStencilAttachment) {
	glGenFramebuffers(1, &id);
	bind();
	
	attach(colorAttachment);
	attach(depthStencilAttachment);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
}

FrameBuffer::~FrameBuffer() {
	Log::warn("Closing framebuffer #%d", id);

	glDeleteFramebuffers(1, &id);
}

void FrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
}

void FrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
}

void FrameBuffer::resize(const Vec2i& dimension) {
	this->dimension = dimension;

	if (texture)
		texture->resize(dimension.x, dimension.y);
	if (renderBuffer)
		renderBuffer->resize(dimension.x, dimension.y);
}

void FrameBuffer::attach(SRef<Texture> texture) {
	bind();
	this->texture = texture;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);
}

void FrameBuffer::attach(SRef<RenderBuffer> renderBuffer) {
	bind();
	this->renderBuffer = renderBuffer;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());
}

void FrameBuffer::close() {
	// TODO remove
}

#pragma endregion

#pragma region FrameBuffer

//! MainFrameBuffer

MainFrameBuffer::MainFrameBuffer(unsigned int width, unsigned int height) : Bindable() {
	glGenFramebuffers(1, &id);
	bind();
	
	fragment = std::make_shared<HDRTexture>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fragment->getID(), 0);

	ca1 = std::make_shared<HDRTexture>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ca1->getID(), 0);

	ca2 = std::make_shared<HDRTexture>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, ca2->getID(), 0);

	ca3 = std::make_shared<HDRTexture>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, ca3->getID(), 0);

	renderBuffer = std::make_shared<RenderBuffer>(width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());

	GLID attachements[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachements);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
}

MainFrameBuffer::~MainFrameBuffer() {
	Log::warn("Closing main framebuffer #%d", id);
	glDeleteFramebuffers(1, &id);
}

void MainFrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
}

void MainFrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
}

void MainFrameBuffer::resize(const Vec2i& dimension) {
	this->dimension = dimension;

	fragment->resize(dimension.x, dimension.y);
	ca1->resize(dimension.x, dimension.y);
	ca2->resize(dimension.x, dimension.y);
	ca3->resize(dimension.x, dimension.y);
	renderBuffer->resize(dimension.x, dimension.y);
}

void MainFrameBuffer::close() {
	// TODO remove
}

#pragma endregion
	
#pragma region HDRFrameBuffer

//! HDRFrameBuffer

HDRFrameBuffer::HDRFrameBuffer(unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &id);
	bind();
	
	texture = std::make_shared<HDRTexture>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getID(), 0);

	renderBuffer = std::make_shared<RenderBuffer>(width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
};

HDRFrameBuffer::~HDRFrameBuffer() {
	Log::warn("Closing HDR framebuffer #%d", id);
	glDeleteFramebuffers(1, &id);
}

void HDRFrameBuffer::resize(const Vec2i& dimension) {
	this->dimension = dimension;
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
	// TODO remove
};

#pragma endregion

#pragma region MultisampleFrameBuffer

//! MultisampleFrameBuffer

MultisampleFrameBuffer::MultisampleFrameBuffer(unsigned int width, unsigned int height, int samples)  {
	glGenFramebuffers(1, &id);
	bind();

	texture = std::make_shared<MultisampleTexture>(width, height, samples);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture->getID(), 0);

	renderBuffer = std::make_shared<MultisampleRenderBuffer>(width, height, samples);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->getID());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log::error("FrameBuffer object with id (%d) not complete", id);

	unbind();
};

MultisampleFrameBuffer::~MultisampleFrameBuffer() {
	Log::warn("Closing multisample framebuffer #%d", id);
	glDeleteFramebuffers(1, &id);
}

void MultisampleFrameBuffer::resize(const Vec2i& dimension) {
	this->dimension = dimension;
	
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
	// TODO remove
};

#pragma endregion

#pragma region DepthFrameBuffer

//! DepthFrameBuffer

DepthFrameBuffer::DepthFrameBuffer(unsigned int width, unsigned int height) : width(width), height(height) {
	glGenFramebuffers(1, &id);
	bind();

	texture = std::make_shared<DepthTexture>(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getID(), 0);
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	unbind();
}

DepthFrameBuffer::~DepthFrameBuffer() {
	Log::warn("Closing depth framebuffer #%d", id);

	glDeleteFramebuffers(1, &id);
}

void DepthFrameBuffer::bind() {
	Renderer::bindFramebuffer(id);
}

void DepthFrameBuffer::unbind() {
	Renderer::bindFramebuffer(0);
}

void DepthFrameBuffer::close() {
	// TODO remove
}

#pragma endregion

}