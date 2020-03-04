#pragma once

#include "../bindable.h"

namespace Graphics {
class Texture;
};
class RenderBuffer;

class FrameBuffer : public Bindable {
public:
	Vec2i dimension;

	Graphics::Texture* texture = nullptr;
	RenderBuffer* renderBuffer = nullptr;
	
	FrameBuffer();
	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(Graphics::Texture* colorAttachment, RenderBuffer* depthStencilAttachment);

	~FrameBuffer();
	FrameBuffer(FrameBuffer&& other);
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(FrameBuffer&& other);
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	void resize(Vec2i dimension);

	void bind() override;
	void unbind() override;
	void close() override;

	void attach(Graphics::Texture* texture);
	void attach(RenderBuffer* renderBuffer);
};

namespace Graphics {
class HDRTexture;
};

class HDRFrameBuffer : public Bindable {
public:
	Vec2i dimension;

	Graphics::HDRTexture* texture = nullptr;
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

namespace Graphics {
class MultisampleTexture;
};
class MultisampleRenderBuffer;

class MultisampleFrameBuffer : public Bindable {
public:
	Vec2i dimension;

	Graphics::MultisampleTexture* texture = nullptr;
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

namespace Graphics {
class DepthTexture;
};

class DepthFrameBuffer : public Bindable {
public:
	unsigned int width;
	unsigned int height;
	Graphics::DepthTexture* texture = nullptr;

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