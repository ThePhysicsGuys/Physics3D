#pragma once

#include "GL\glew.h"
#include "../util/log.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define glCall(x) clearError(); x; ASSERT(logCall(#x, __FILE__, __LINE__));

void clearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool logCall(const char* func, const char* file, int line) {
	bool response = true;
	unsigned int error = glGetError();
	while (error != GL_NO_ERROR) {
		Log::error("[OpenGL error 0x%x] %s:%d at %s", error, file, line, func);
		error = glGetError();
		response =  false;
	}
	return response;
}
