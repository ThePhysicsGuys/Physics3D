#pragma once

#include "../buffers/bufferLayout.h"

namespace P3D::Graphics {

struct BatchConfig {
	// Layout of the vertex buffer
	BufferLayout bufferLayout;
	// Primitive type
	unsigned int type;

	BatchConfig(BufferLayout bufferLayout, unsigned int type) : bufferLayout(bufferLayout), type(type) {};
};

};