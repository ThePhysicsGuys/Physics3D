#pragma once

#include "bindable.h"
#include "gui/color.h"

namespace P3D::Graphics {

class Texture : public Bindable {
private:
	static SRef<Texture> _white;

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
	Texture();
	Texture(int width, int height);
	Texture(int width, int height, const void* buffer, int format);
	Texture(int width, int height, const void* buffer, int target, int format, int internalFormat, int type);

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

	SRef<Texture> colored(const Color& color);
	void generateMipmap();

	static Texture load(const std::string& name);
	static SRef<Texture> white();

	[[nodiscard]] float getAspect() const;
	[[nodiscard]] int getWidth() const;
	[[nodiscard]] int getHeight() const;
	[[nodiscard]] int getInternalFormat() const;
	[[nodiscard]] int getFormat() const;
	[[nodiscard]] int getTarget() const;
	[[nodiscard]] int getType() const;
	[[nodiscard]] int getChannels() const;
	[[nodiscard]] int getUnit() const;

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

};