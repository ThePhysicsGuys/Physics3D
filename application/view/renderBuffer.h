#pragma once

#include "bindable.h"

class RenderBuffer : public Bindable {
public:
	unsigned int width;
	unsigned int height;

	RenderBuffer(unsigned int width, unsigned int height);

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

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);

};