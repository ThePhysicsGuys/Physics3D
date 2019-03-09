#pragma once

#include "bindable.h"

class Texture : public Bindable {
public:
	unsigned int width;
	unsigned int height;

	Texture(unsigned int width, unsigned int height);

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);

};

class TextureMultisample : public Bindable {
	unsigned int width;
	unsigned int height;
	unsigned int samples;

	TextureMultisample(unsigned int width, unsigned int height, unsigned int samples);

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
};