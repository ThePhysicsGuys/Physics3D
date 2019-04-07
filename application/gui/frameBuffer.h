#pragma once

#include "texture.h"
#include "renderBuffer.h"
#include "bindable.h"

class FrameBuffer : public Bindable {
public:
	Texture* texture = nullptr;
	RenderBuffer* renderBuffer = nullptr;
	
	FrameBuffer();
	FrameBuffer(unsigned int width, unsigned int height);
	FrameBuffer(Texture* colorAttachment, RenderBuffer* depthStencilAttachment);

	void bind() override;
	void unbind() override;
	void close() override;

	void attach(Texture* texture); 
	void attach(RenderBuffer* renderBuffer);
};

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