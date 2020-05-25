#pragma once

#include "bindable.h"

typedef unsigned int GLFLAG;

namespace Graphics {

namespace Renderer {

// GL constants

extern GLFLAG WIREFRAME;
extern GLFLAG FILL;
extern GLFLAG POINT;
extern GLFLAG FRONT_AND_BACK;

extern GLFLAG STATIC_DRAW;
extern GLFLAG DYNAMIC_DRAW;
extern GLFLAG STREAM_DRAW;

extern GLFLAG TRIANGLES;
extern GLFLAG PATCHES;
extern GLFLAG QUADS;
extern GLFLAG LINES;
extern GLFLAG POINTS;

extern GLFLAG UINT;

struct RenderState {
	GLID dfbo;
	GLID rfbo;
	GLID rbo;
	GLID texture;
	GLID program;
	GLID vao;
	GLID vbo;
	GLID ibo;
	int activeTexture;
	int viewport[4];
	int mode[2];
	bool blend;
	bool cull;
	bool depth;
	bool scissor;
	bool depthmask;
};

// GLEW binding

extern bool initGLEW();

extern void clearDepth();
extern void clearColor();
extern void clearStencil();

extern void lineStipple(int factor, short pattern);
extern void lineWidth(float size);

extern void viewport(Vec2i origin, Vec2i dimension);

extern void enableDepthMask();
extern void disableDepthMask();
extern void enableDepthTest();
extern void disableDepthTest();
extern void enableCulling();
extern void disableCulling();
extern void enableBlending();
extern void disableBlending();
extern void standardBlendFunction();
extern void enableMultisampling();

extern const unsigned char* getVendor();
extern const unsigned char* getVersion();
extern const unsigned char* getRenderer();
extern const unsigned char* getShaderVersion();

extern void bindShader(GLID id);
extern void bindTexture2D(GLID id);
extern void bindFramebuffer(GLID id);
extern void bindDrawbuffer(GLID id);
extern void bindReadbuffer(GLID id);
extern void bindRenderbuffer(GLID id);

extern void polygonMode(int face, int mode);
extern void scissor(int x, int y, int width, int height);
extern void drawElementsInstanced(GLFLAG mode, size_t count, GLFLAG type, const void* offset, size_t primitives);
extern void drawElements(GLFLAG mode, size_t count, GLFLAG type, const void* offset);
extern void drawArrays(GLFLAG mode, int first, size_t count);

extern void defaultSettings();

extern RenderState getState();
extern void beginScene();
extern void endScene();

}

};