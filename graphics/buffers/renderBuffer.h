#pragma once

#include "../bindable.h"

namespace P3D::Graphics {

class RenderBuffer : public Bindable {
public:
	unsigned int width;
	unsigned int height;

	RenderBuffer(unsigned int width, unsigned int height);

	~RenderBuffer();
	RenderBuffer(RenderBuffer&& other);
	RenderBuffer(const RenderBuffer&) = delete;
	RenderBuffer& operator=(RenderBuffer&& other);
	RenderBuffer& operator=(const RenderBuffer&) = delete;

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);

};

class MultisampleRenderBuffer : public Bindable {
public:
	unsigned int width;
	unsigned int height;
	unsigned int samples;

	MultisampleRenderBuffer(unsigned int width, unsigned int height, unsigned int samples);

	~MultisampleRenderBuffer();
	MultisampleRenderBuffer(MultisampleRenderBuffer&& other);
	MultisampleRenderBuffer(const MultisampleRenderBuffer&) = delete;
	MultisampleRenderBuffer& operator=(MultisampleRenderBuffer&& other);
	MultisampleRenderBuffer& operator=(const MultisampleRenderBuffer&) = delete;

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
};

};