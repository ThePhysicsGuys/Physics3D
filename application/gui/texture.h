#pragma once

#include "bindable.h"
#include <vector>

class Texture : public Bindable {
public:
	int unit;
	unsigned int width;
	unsigned int height;

	Texture(unsigned int width, unsigned int height, const void* buffer, int format);
	Texture(unsigned int width, unsigned int height) : Texture(width, height, nullptr,
		0x1908) {};

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
	void resize(unsigned int width, unsigned int height, const void* buffer);

};

class TextureMultisample : public Bindable {
public:
	int unit;
	unsigned int width;
	unsigned int height;
	unsigned int samples;

	TextureMultisample(unsigned int width, unsigned int height, unsigned int samples);

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
};

class CubeMap : public Bindable {
public:
	int unit;

	CubeMap(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);

	void load(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;
};