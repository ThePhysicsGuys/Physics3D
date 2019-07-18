#pragma once

#include "GL\glew.h"

#include "../util/log.h"

#include <vector>

struct BufferElement {
	unsigned int type;
	unsigned int count;
	unsigned int normalized;
	unsigned int size;
};

class BufferLayout {
public:
	std::vector<BufferElement> elements;
	unsigned int stride;

	BufferLayout() : stride(0) {};

	~BufferLayout() {
		Log::debug("Deleted buffer layout");
	}

	template<typename T>
	void push(unsigned int count) {};

	template<>
	void push<float>(unsigned int count) {
		elements.push_back({ GL_FLOAT, count, GL_FALSE , sizeof(float) });
		stride += sizeof(float) * count;
	}

	template<>
	void push<unsigned int>(unsigned int count) {
		elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE , sizeof(unsigned int) });
		stride += sizeof(unsigned int) * count;
	}
};