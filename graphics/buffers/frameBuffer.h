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

	SRef<Texture> texture;
	SRef<RenderBuffer> renderBuffer;

	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(SRef<Texture> colorAttachment, SRef<RenderBuffer> depthStencilAttachment);
	virtual ~FrameBuffer();

	void bind() override;
	void unbind() override;
	void close() override;
	
	void resize(const Vec2i& dimension);
	void attach(SRef<Texture> texture);
	void attach(SRef<RenderBuffer> renderBuffer);
};

class MainFrameBuffer : public Bindable {
private:
	MainFrameBuffer();

public:
	Vec2i dimension;

	SRef<Texture> fragment;
	SRef<Texture> ca1;
	SRef<Texture> ca2;
	SRef<Texture> ca3;
	SRef<RenderBuffer> renderBuffer;

	MainFrameBuffer(unsigned int width, unsigned int height);
	virtual ~MainFrameBuffer();

	void bind() override;
	void unbind() override;
	void close() override;
	
	void resize(const Vec2i& dimension);
};

class HDRTexture;

class HDRFrameBuffer : public Bindable {
public:
	Vec2i dimension;

	SRef<HDRTexture> texture;
	SRef<RenderBuffer> renderBuffer;

	HDRFrameBuffer();
	HDRFrameBuffer(unsigned int width, unsigned int height);
	virtual ~HDRFrameBuffer();
	
	void bind() override;
	void unbind() override;
	void close() override;

	void resize(const Vec2i& dimension);
};


class MultisampleTexture;
class MultisampleRenderBuffer;

class MultisampleFrameBuffer : public Bindable {
public:
	Vec2i dimension;

	SRef<MultisampleTexture> texture;
	SRef<MultisampleRenderBuffer> renderBuffer;

	MultisampleFrameBuffer();
	MultisampleFrameBuffer(unsigned int width, unsigned int height, int samples);
	~MultisampleFrameBuffer();

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(const Vec2i& dimension);
};

class DepthTexture;

class DepthFrameBuffer : public Bindable {
public:
	unsigned int width;
	unsigned int height;
	SRef<DepthTexture> texture;

	DepthFrameBuffer(unsigned int width, unsigned int height);
	~DepthFrameBuffer();

	void bind() override;
	void unbind() override;
	void close() override;
};

};