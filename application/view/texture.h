#pragma once

#include "bindable.h"

class Texture : public Bindable {
public:
	int unit;
	unsigned int width;
	unsigned int height;
	
	int format;
	int channels;

	Texture(unsigned int width, unsigned int height, const void* buffer, int format);
	Texture(unsigned int width, unsigned int height);

	~Texture();
	Texture(Texture&& other);
	Texture(const Texture&) = delete;
	Texture& operator=(Texture&& other);
	Texture& operator=(const Texture&) = delete;

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
	void resize(unsigned int width, unsigned int height, const void* buffer);

	void loadFrameBufferTexture(unsigned int width, unsigned int height);

	Texture* colored(Vec3 color);
	Texture* colored(Vec4 color);
};

Texture* load(const std::string& name);

class HDRTexture : public Bindable {
public:
	int unit;
	unsigned int width;
	unsigned int height;

	HDRTexture(unsigned int width, unsigned int height, const void* buffer);
	HDRTexture(unsigned int width, unsigned int height);

	~HDRTexture();
	HDRTexture(HDRTexture&& other);
	HDRTexture(const HDRTexture&) = delete;
	HDRTexture& operator=(HDRTexture&& other);
	HDRTexture& operator=(const HDRTexture&) = delete;

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
	void resize(unsigned int width, unsigned int height, const void* buffer);
};

class MultisampleTexture : public Bindable {
public:
	int unit;
	unsigned int width;
	unsigned int height;
	unsigned int samples;

	MultisampleTexture(unsigned int width, unsigned int height, unsigned int samples);

	~MultisampleTexture();
	MultisampleTexture(MultisampleTexture&& other);
	MultisampleTexture(const MultisampleTexture&) = delete;
	MultisampleTexture& operator=(MultisampleTexture&& other);
	MultisampleTexture& operator=(const MultisampleTexture&) = delete;

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;

	void resize(unsigned int width, unsigned int height);
};

class CubeMap : public Bindable {
public:
	int unit;

	CubeMap(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back);

	~CubeMap();
	CubeMap(CubeMap&& other);
	CubeMap(const CubeMap&) = delete;
	CubeMap& operator=(CubeMap&& other);
	CubeMap& operator=(const CubeMap&) = delete;

	void load(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back);

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;
};

class DepthTexture : public Bindable {
public:
	int unit;
	unsigned int width;
	unsigned int height;

	DepthTexture(unsigned int width, unsigned int height);

	~DepthTexture();
	DepthTexture(DepthTexture&& other);
	DepthTexture(const DepthTexture&) = delete;
	DepthTexture& operator=(DepthTexture&& other);
	DepthTexture& operator=(const DepthTexture&) = delete;

	void bind(int unit);
	void bind() override;
	void unbind() override;
	void close() override;
};
