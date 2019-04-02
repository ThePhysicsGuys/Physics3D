#include "texture.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../util/log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"

Texture::Texture(unsigned int width, unsigned int height, const void* buffer, int format) : width(width), height(height), unit(0) {
	glGenTextures(1, &id);

	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unbind();
}

void Texture::resize(unsigned int width, unsigned int height, const void* buffer) {
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	unbind();
}

void Texture::resize(unsigned int width, unsigned int height) {
	resize(width, height, nullptr);
}

void Texture::bind(int unit) {
	this->unit = unit;
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::bind() {
	bind(unit);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::close() {
	glDeleteTextures(1, &id);
}

TextureMultisample::TextureMultisample(unsigned int width, unsigned int height, unsigned int samples) : width(width), height(height), samples(samples), unit(0) {
	glGenTextures(1, &id);
	bind();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unbind();
}

void TextureMultisample::resize(unsigned int width, unsigned int height) {
	bind();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	unbind();
}

void TextureMultisample::bind() {
	bind(unit);
}

void TextureMultisample::bind(int unit) {
	this->unit = unit;
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
}

void TextureMultisample::unbind() {
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void TextureMultisample::close() {
	glDeleteTextures(1, &id);
}

CubeMap::CubeMap(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back) : unit(0) {
	glGenTextures(1, &id);
	bind();
	
	load(right, left, top, bottom, front, back);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	bind();
}

void CubeMap::bind() {
	bind(unit);
}

void CubeMap::bind(int unit) {
	this->unit = unit;
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void CubeMap::unbind() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void CubeMap::load(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back) {
	int width;
	int height;
	int channels;
	unsigned char* data;
	std::string faces[6] = { right, left, top, bottom, front, back };

	for (int i = 0; i < 6; i++) {
		data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
		if (data)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			Log::error("Failed to load: %s", faces[i].c_str());
		
		stbi_image_free(data);	
	}
}

void CubeMap::close() {
	glDeleteTextures(1, &id);
}