#pragma once

#include <vector>
#include "GL\glew.h"

struct BufferElement {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	unsigned char size;
};

class BufferLayout {
public:
	std::vector<BufferElement> elements;
	unsigned int stride;

	BufferLayout() : stride(0) {};

	template<typename T>
	void push(unsigned int count) {};

	template<>
	void push<double>(unsigned int count) {
		elements.push_back({ GL_DOUBLE, count, GL_FALSE , sizeof(double) });
		stride += sizeof(double);
	}

	template<>
	void push<unsigned int>(unsigned int count) {
		elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE , sizeof(unsigned int) });
		stride += sizeof(unsigned int);
	}
};