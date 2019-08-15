#pragma once

#include "buffers\bufferLayout.h"

struct BatchConfig {
	BufferLayout bufferLayout;

	BatchConfig(BufferLayout bufferLayout) : bufferLayout(bufferLayout) {};
};