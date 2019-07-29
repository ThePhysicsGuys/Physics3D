#pragma once

#include "../bindable.h"

#include "../engine/math/vec2.h"

class Texture;
class RenderBuffer;

class FrameBuffer : public Bindable {
public:
	Vec2i dimension;

	Texture* texture = nullptr;
	RenderBuffer* renderBuffer = nullptr;
	
	FrameBuffer();
	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(Texture* colorAttachment, RenderBuffer* depthStencilAttachment);

	void resize(Vec2i dimension);

	void bind() override;
	void unbind() override;
	void close() override;

	void attach(Texture* texture); 
	void attach(RenderBuffer* renderBuffer);
};


class HDRTexture;

class HDRFrameBuffer : public Bindable {
public:
	Vec2i dimension;

	HDRTexture* texture = nullptr;
	RenderBuffer* renderBuffer = nullptr;

	HDRFrameBuffer();
	HDRFrameBuffer(unsigned int width, unsigned int height);

	void resize(Vec2i dimension);

	void bind() override;
	void unbind() override;
	void close() override;
};


class MultisampleTexture;
class MultisampleRenderBuffer;

class MultisampleFrameBuffer : public Bindable {
public:
	Vec2i dimension;

	MultisampleTexture* texture = nullptr;
	MultisampleRenderBuffer* renderBuffer = nullptr;

	MultisampleFrameBuffer();
	MultisampleFrameBuffer(unsigned int width, unsigned int height, int samples);

	void resize(Vec2i dimension);

	void bind() override;
	void unbind() override;
	void close() override;
};


class DepthTexture;

class DepthFrameBuffer : public Bindable {
public:
	unsigned int width;
	unsigned int height;
	DepthTexture* texture = nullptr;

	DepthFrameBuffer(unsigned int width, unsigned int height);

	void bind() override;
	void unbind() override;
	void close() override;
};