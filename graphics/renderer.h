#pragma once

#include "bindable.h"

typedef unsigned int GLFLAG;

namespace P3D::Graphics {

namespace Renderer {

// GL constants

extern GLFLAG WIREFRAME;
extern GLFLAG FILL;
extern GLFLAG POINT;
extern GLFLAG FRONT_AND_BACK;

extern GLFLAG STATIC_DRAW;
extern GLFLAG DYNAMIC_DRAW;
extern GLFLAG STREAM_DRAW;

extern GLFLAG ARRAY_BUFFER;

extern GLFLAG TRIANGLES;
extern GLFLAG PATCHES;
extern GLFLAG QUADS;
extern GLFLAG LINE_STRIP;
extern GLFLAG LINES;
extern GLFLAG POINTS;

extern GLFLAG UINT;
extern GLFLAG FLOAT;

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

extern void viewport(const Vec2i& origin, const Vec2i& dimension);

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

extern const char* getVendor();
extern const char* getVersion();
extern const char* getRenderer();
extern const char* getShaderVersion();
extern int getMaxTextureUnits();
extern int parseShaderVersion(const char* version);

extern void genBuffers(int count, GLID* id);
extern void genVertexArrays(int count, GLID* id);
void delBuffers(int count, GLID* id);
void delVertexArrays(int count, GLID* id);
extern void bindBuffer(GLFLAG target, GLID id);
extern void bufferData(GLFLAG target, int size, int offset, GLFLAG type);
extern void bufferSubData(GLFLAG target, int offset, int size, const void* pointer);

extern void enableVertexAttribArray(GLID id);
extern void vertexAttribPointer(GLID id, int size, GLFLAG type, bool normalized, int stride, const void* pointer);
	
extern void bindShader(GLID id);
extern void bindTexture2D(GLID id);
extern void bindFramebuffer(GLID id);
extern void bindDrawbuffer(GLID id);
extern void bindReadbuffer(GLID id);
extern void bindRenderbuffer(GLID id);
extern void bindVertexArray(GLID id);

void activeTexture(GLID unit);

extern void polygonMode(int face, int mode);
extern void scissor(int x, int y, int width, int height);
extern void drawElementsInstanced(GLFLAG mode, size_t count, GLFLAG type, const void* offset, size_t primitives);
extern void drawElements(GLFLAG mode, size_t count, GLFLAG type, const void* offset);
extern void drawArrays(GLFLAG mode, int first, size_t count);

extern void defaultSettings(GLID defaultFrameBuffer);

extern RenderState getState();
extern void beginScene();
extern void endScene();

}

};