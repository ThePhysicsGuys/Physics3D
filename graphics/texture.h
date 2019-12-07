#pragma once

#include "bindable.h"
#include "gui/color.h"

class Texture : public Bindable {
protected:
	int width;
	int height;
	int internalFormat;
	int format;
	int target;
	int type;

	int unit;
	int channels;

	virtual void create(int target, int level, int internalFormat, int width, int height, int border, int format, int type, const void* buffer);

public:
	Texture(int width, int height, const void* buffer, int target, int format, int internalFormat, int type);
	Texture(int width, int height, const void* buffer, int format);
	Texture(int width, int height);
	Texture();

	~Texture();
	Texture(Texture&& other);
	Texture(const Texture&) = delete;
	Texture& operator=(Texture&& other);
	Texture& operator=(const Texture&) = delete;

	virtual void bind(int unit);
	virtual void bind() override;
	virtual void unbind() override;
	virtual void close() override;

	virtual void resize(int width, int height);
	virtual void resize(int width, int height, const void* buffer);

	void loadFrameBufferTexture(int width, int height);

	Texture* colored(Color3 color);
	Texture* colored(Color color);

	static Texture load(const std::string& name);

	int getWidth() const;
	int getHeight() const;
	int getInternalFormat() const;
	int getFormat() const;
	int getTarget() const;
	int getType() const;
	int getChannels() const;
	int getUnit() const;

	void setUnit(int unit);
};


class HDRTexture : public Texture {
public:
	HDRTexture(int width, int height, const void* buffer);
	HDRTexture(int width, int height);
};

class MultisampleTexture : public Texture {
protected:
	int samples;

	virtual void create(int target, int level, int internalFormat, int width, int height, int border, int format, int type, const void* buffer) override;

public:
	MultisampleTexture(int width, int height, int samples);

	void resize(int width, int height) override;
};

class CubeMap : public Texture {
protected:
	virtual void create(int target, int level, int internalFormat, int width, int height, int border, int format, int type, const void* buffer) override;

public:
	CubeMap(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back);

	void load(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back);

	void resize(int width, int height) override;
};

class DepthTexture : public Texture {
public:
	DepthTexture(int width, int height);
};