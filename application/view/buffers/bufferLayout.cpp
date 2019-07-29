#include "bufferLayout.h"

#include "GL\glew.h"
#include "../util/log.h"

void BufferLayout::pushFloat(unsigned int count) {
	elements.push_back({ GL_FLOAT, count, GL_FALSE , sizeof(float) });
	stride += sizeof(float) * count;
}

void BufferLayout::pushInt(unsigned int count) {
	elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE , sizeof(unsigned int) });
	stride += sizeof(unsigned int) * count;
}

BufferLayout::~BufferLayout() {
	Log::debug("Deleted buffer layout");
}