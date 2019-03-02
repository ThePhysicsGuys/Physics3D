#pragma once

class Texture {
public:
	unsigned int id;
	unsigned int width;
	unsigned int height;

	Texture(unsigned int width, unsigned int height);

	void bind();
	void unbind();

	void resize(unsigned int width, unsigned int height);

	void close();
};