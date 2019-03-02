#pragma once

#include "texture.h"
#include "renderBuffer.h"

class FrameBuffer {
public:
	unsigned int id;

	FrameBuffer();
	FrameBuffer(Texture colorAttachment, RenderBuffer depthStencilAttachment);

	void bind();
	void unbind();

	void attach(Texture texture); 
	void attach(RenderBuffer renderBuffer);

	void close();
};