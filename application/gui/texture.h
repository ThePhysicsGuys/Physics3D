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