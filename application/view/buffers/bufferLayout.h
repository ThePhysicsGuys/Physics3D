#pragma once

#include <vector>

struct BufferElement {
	unsigned int type;
	unsigned int count;
	unsigned int normalized;
	unsigned int size;
};

struct BufferLayout {
public:
	std::vector<BufferElement> elements;
	unsigned int stride;

	BufferLayout() : stride(0) {};
	~BufferLayout();

	void pushFloat(unsigned int count);
	void pushInt(unsigned int count);
};