#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../engine/math/vec.h"

namespace Renderer {

	// GL constants

	extern unsigned int WIREFRAME;
	extern unsigned int FILL;
	extern unsigned int POINT;

	extern unsigned int STATIC_DRAW;
	extern unsigned int DYNAMIC_DRAW;
	extern unsigned int STREAM_DRAW;

	extern unsigned int TRIANGLES;
	extern unsigned int PATCHES;
	extern unsigned int QUADS;
	extern unsigned int LINES;
	extern unsigned int POINTS;

	extern unsigned int UINT;

	// GLFW binding

	extern inline bool initGLFW();
	extern inline void terminateGLFW();

	extern inline GLFWwindow* getGLFWContext();
	extern inline void createGLFWContext(int width, int height, const char* title);
	extern inline void makeGLFWContextCurrent();
	extern inline bool validGLFWContext();

	extern inline void swapGLFWInterval(int interval);
	extern inline void swapGLFWBuffers();
	extern inline void pollGLFWEvents();

	extern inline void closeGLFWWindow();
	extern inline bool isGLFWWindowClosed();

	extern inline Vec2i getGLFWWindowSize();
	extern inline Vec4i getGLFWFrameSize();
	
	extern inline void enableGLFWCursor();
	extern inline void disableGLFWCursor();

	extern inline void setGLFWMultisampleSamples(int samples);


	// GLEW binding

	extern bool initGLEW();

	extern void clearDepth();
	extern void clearColor();
	extern void clearStencil();

	extern void lineStipple(int factor, short pattern);
	extern void lineWidth(float size);

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

	extern inline void enableMultisampling();

	extern const unsigned char* getVendor();
	extern const unsigned char* getVersion();
	extern const unsigned char* getRenderer();
	extern const unsigned char* getShaderVersion();

	extern inline void bindTexture2D(int id);
	extern inline void scissor(int x, int y, int width, int height);
	extern inline void drawElements(unsigned int mode, size_t count, unsigned int type, const void * offset);
	extern inline void drawArrays(unsigned int mode, int first, size_t count);
}