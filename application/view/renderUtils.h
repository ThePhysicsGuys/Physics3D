#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../engine/math/vec2.h"
#include "../engine/math/vec4.h"

namespace Renderer {

	extern unsigned int WIREFRAME;
	extern unsigned int FILLED;
	extern unsigned int POINTS;

	extern bool initGLEW();
	extern bool initGLFW();
	extern void terminateGLFW();
	extern void createContext(int width, int height, const char* title);
	extern void makeContextCurrent();
	extern bool validContext();

	extern void swapInterval(int interval);
	extern void swapBuffers();
	extern void pollEvents();

	extern void clearDepth();
	extern void clearColor();
	extern void clearStencil();

	// To remove
	extern GLFWwindow* getContext();

	extern inline void closeWindow();
	extern inline bool windowClosed();

	extern inline Vec2i windowSize();
	extern inline Vec4i frameSize();
	extern inline void viewport(Vec2i origin, Vec2i dimension);

	extern inline void enableDepthMask();
	extern inline void disableDepthMask();
	extern inline void enableDepthTest();
	extern inline void disableDepthTest();

	extern inline void enableCulling();
	extern inline void disableCulling();
	 
	extern inline void enableBlending();
	extern inline void disableBlending();
	extern inline void standardBlendFunction();

	extern inline void enableCursor();
	extern inline void disableCursor();

	extern const unsigned char* getVendor();
	extern const unsigned char* getVersion();
	extern const unsigned char* getRenderer();
	extern const unsigned char* getShaderVersion();
}