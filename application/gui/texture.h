#pragma once

#include "bindable.h"
#include <vector>

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
public:
	unsigned int width;
	unsigned int height;
	unsigned int samples;

	TextureMultisample(unsigned int width, unsigned int height, unsigned int samples);

	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
};

class CubeMap : public Bindable {
public:
	CubeMap(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);

	void load(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);

	void bind() override;
	void unbind() override;
	void close() override;
};