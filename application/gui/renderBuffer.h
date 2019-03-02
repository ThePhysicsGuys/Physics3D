#pragma once

class RenderBuffer {
public:
	unsigned int id;
	unsigned int width;
	unsigned int height;

	RenderBuffer(unsigned int width, unsigned int height);

	void bind();
	void unbind();

	void resize(unsigned int width, unsigned int height);

	void close();
};