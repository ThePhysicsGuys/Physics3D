#include "core.h"

#include "texture.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"

#include "debug/debug.h"

namespace Graphics {

#pragma region Texture

//! Texture

Texture* Texture::_white = nullptr;

int getChannelsFromFormat(int format) {
	switch (format) {
		case GL_RED:
		case GL_GREEN:
		case GL_BLUE:
		case GL_ALPHA:
			return 1;
			break;
		case GL_RGB:
			return 3;
			break;
		case GL_RGBA:
			return 4;
			break;
		default:
			Log::warn("Unknown format: %x, assuming 4 channels", format);
			return 4;
	}

}

int getFormatFromChannels(int channels) {
	switch (channels) {
		case 1:
			return GL_RED;
			break;
		case 3:
			return GL_RGB;
			break;
		case 4:
			return GL_RGBA;
			break;
		default:
			Log::warn("Unknown amount of channels: %d, assuming RGB", channels);
			return GL_RGB;
	}
}

Texture Texture::load(const std::string& name) {
	int width;
	int height;
	int channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(name.c_str(), &width, &height, &channels, 0);

	if (data) {
		int format = getFormatFromChannels(channels);

		Texture texture(width, height, data, format);

		stbi_image_free(data);

		return texture;
	} else {
		Log::subject s(name);
		Log::error("Failed to load texture");

		stbi_image_free(data);

		return Texture();
	}
}

Texture* Texture::white() {
	if (_white == nullptr) {
		Color buffer = COLOR::WHITE;
		_white = new Texture(1, 1, &buffer, GL_RGBA);
	}

	return _white;
}

Texture::Texture() : width(0), height(0), internalFormat(0), format(0), target(0), type(0), unit(0), channels(0) {

}

Texture::Texture(int width, int height) : Texture(width, height, nullptr, GL_RGBA) {

}

Texture::Texture(int width, int height, const void* buffer, int format) : Texture(width, height, buffer, GL_TEXTURE_2D, format, format, GL_UNSIGNED_BYTE) {

}

Texture::Texture(int width, int height, const void* buffer, int target, int format, int internalFormat, int type) : width(width), height(height), target(target), format(format), internalFormat(internalFormat), type(type), unit(0) {
	glGenTextures(1, &id);

	channels = getChannelsFromFormat(format);

	bind();
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	create(target, 0, internalFormat, width, height, 0, format, type, buffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	unbind();
}

void Texture::create(int target, int level, int internalFormat, int width, int height, int border, int format, int type, const void* buffer) {
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, buffer);
}

Texture::~Texture() {
	close();
}

Texture::Texture(Texture&& other) : Bindable(other.id), width(other.width), height(other.height), internalFormat(other.internalFormat), format(other.format), target(other.target), type(other.type), unit(other.unit), channels(other.channels) {
	other.id = 0;
	other.width = 0;
	other.height = 0;
	other.type = 0;
	other.internalFormat = 0;
	other.format = 0;
	other.channels = 0;
	other.type = 0;
	other.unit = 0;
}

Texture& Texture::operator=(Texture&& other) {
	if (this != &other) {
		id = 0;
		width = 0;
		height = 0;
		type = 0;
		internalFormat = 0;
		format = 0;
		channels = 0;
		type = 0;
		unit = 0;

		std::swap(id, other.id);
		std::swap(width, other.width);
		std::swap(height, other.height);
		std::swap(internalFormat, other.internalFormat);
		std::swap(format, other.format);
		std::swap(target, other.target);
		std::swap(type, other.type);
		std::swap(unit, other.unit);
		std::swap(channels, other.channels);
	}

	return *this;
}

void Texture::loadFrameBufferTexture(int width, int height) {
	bind();
	glCopyTexImage2D(target, 0, internalFormat, 0, 0, width, height, 0);
	unbind();
}

void Texture::resize(int width, int height, const void* buffer) {
	bind();
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, buffer);
	unbind();
}

Texture* Texture::colored(Color3 color) {
	return colored(Color(color.x, color.y, color.z, 1));
}

Texture* Texture::colored(Color color) {
	bind();
	unsigned char* buffer = (unsigned char*) malloc(width * height * channels);

	glGetTexImage(target, 0, format, type, buffer);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			for (int k = 0; k < channels; k++) {

				int index = (i + height * j) * channels + k;
				unsigned char value = unsigned char(buffer[index] * color[k]);
				buffer[index] = value;
			}
		}
	}

	Texture* texture = new Texture(width, height, buffer, format);

	free(buffer);

	return texture;
}

void Texture::resize(int width, int height) {
	resize(width, height, nullptr);
}

void Texture::bind(int unit) {
	this->unit = unit;
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, id);
}

void Texture::bind() {
	bind(unit);
}

void Texture::unbind() {
	glBindTexture(target, 0);
}

void Texture::close() {
	if (id != 0) {
		Log::warn("Closed texture #%d", id);

		glDeleteTextures(1, &id);
		id = 0;
	}
}

float Texture::getAspect() const {
	return ((float) width) / ((float) height);
}

int Texture::getWidth() const {
	return width;
}

int Texture::getHeight() const {
	return height;
}

int Texture::getInternalFormat() const {
	return internalFormat;
}

int Texture::getFormat() const {
	return format;
}

int Texture::getTarget() const {
	return target;
}

int Texture::getType() const {
	return type;
}

int Texture::getChannels() const {
	return channels;
}

int Texture::getUnit() const {
	return unit;
}

void Texture::setUnit(int unit) {
	this->unit = unit;
}

#pragma endregion

#pragma region HDRTexture

//! HDRTexture

HDRTexture::HDRTexture(int width, int height) : HDRTexture(width, height, nullptr) {

};

HDRTexture::HDRTexture(int width, int height, const void* buffer) : Texture(width, height, buffer, GL_TEXTURE_2D, GL_RGBA, GL_RGBA16F, GL_FLOAT) {

}

#pragma endregion

#pragma region TextureMultisample

//! TextureMultisample

MultisampleTexture::MultisampleTexture(int width, int height, int samples) : Texture(width, height, nullptr, GL_TEXTURE_2D_MULTISAMPLE, 0, GL_RGBA, 0), samples(samples) {
	glGenTextures(1, &id);
	bind();
	create(target, 0, internalFormat, width, height, 0, format, type, nullptr);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unbind();
}

void MultisampleTexture::create(int target, int level, int internalFormat, int width, int height, int border, int format, int type, const void* buffer) {
	glTexImage2DMultisample(target, samples, internalFormat, width, height, GL_TRUE);
}

void MultisampleTexture::resize(int width, int height) {
	bind();
	glTexImage2DMultisample(target, samples, internalFormat, width, height, GL_TRUE);
	unbind();
}

#pragma endregion

#pragma region CubeMap

//! CubeMap

CubeMap::CubeMap(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back) : Texture(0, 0, nullptr, GL_TEXTURE_CUBE_MAP, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE) {
	bind();
	load(right, left, top, bottom, front, back);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	unbind();
}

void CubeMap::create(int target, int level, int internalFormat, int width, int height, int border, int format, int type, const void* buffer) {

}

void CubeMap::load(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& front, const std::string& back) {
	unsigned char* data;
	std::string faces[6] = { right, left, top, bottom, front, back };

	stbi_set_flip_vertically_on_load(false);

	for (int i = 0; i < 6; i++) {
		data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

		format = internalFormat = getFormatFromChannels(channels);

		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, internalFormat, type, data);
		else {
			Log::subject s(faces[i]);
			Log::error("Failed to load texture");
		}

		stbi_image_free(data);
	}
}

void CubeMap::resize(int width, int height) {
	Log::error("Resizing of cubemap not supported yet");
}

#pragma endregion

#pragma region DepthTexture

//! DepthTexture

DepthTexture::DepthTexture(int width, int height) : Texture(width, height, nullptr, GL_TEXTURE_2D, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT) {

}

#pragma endregion

};