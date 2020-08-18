#pragma once

#include "../bindable.h"

namespace P3D::Graphics {

class Texture;
class RenderBuffer;

class FrameBuffer : public Bindable {
private:
	FrameBuffer();

public:
	Vec2i dimension;

	Texture* texture = nullptr;
	RenderBuffer* renderBuffer = nullptr;

	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(Texture* colorAttachment, RenderBuffer* depthStencilAttachment);

	~FrameBuffer();
	FrameBuffer(FrameBuffer&& other);
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(FrameBuffer&& other);
	FrameBuffer& operator=(const FrameBuffer&) = delete;

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

	~HDRFrameBuffer();
	HDRFrameBuffer(HDRFrameBuffer&& other);
	HDRFrameBuffer(const HDRFrameBuffer&) = delete;
	HDRFrameBuffer& operator=(HDRFrameBuffer&& other);
	HDRFrameBuffer& operator=(const HDRFrameBuffer&) = delete;

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

	~MultisampleFrameBuffer();
	MultisampleFrameBuffer(MultisampleFrameBuffer&& other);
	MultisampleFrameBuffer(const MultisampleFrameBuffer&) = delete;
	MultisampleFrameBuffer& operator=(MultisampleFrameBuffer&& other);
	MultisampleFrameBuffer& operator=(const MultisampleFrameBuffer&) = delete;

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

	~DepthFrameBuffer();
	DepthFrameBuffer(DepthFrameBuffer&& other);
	DepthFrameBuffer(const DepthFrameBuffer&) = delete;
	DepthFrameBuffer& operator=(DepthFrameBuffer&& other);
	DepthFrameBuffer& operator=(const DepthFrameBuffer&) = delete;

	void bind() override;
	void unbind() override;
	void close() override;
};

};