#pragma once

#include "bindable.h"

namespace Graphics {

namespace Renderer {

// GL constants

extern unsigned int WIREFRAME;
extern unsigned int FILL;
extern unsigned int POINT;
extern unsigned int FRONT_AND_BACK;

extern unsigned int STATIC_DRAW;
extern unsigned int DYNAMIC_DRAW;
extern unsigned int STREAM_DRAW;

extern unsigned int TRIANGLES;
extern unsigned int PATCHES;
extern unsigned int QUADS;
extern unsigned int LINES;
extern unsigned int POINTS;

extern unsigned int UINT;

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
extern void drawElements(unsigned int mode, size_t count, unsigned int type, const void* offset);
extern void drawArrays(unsigned int mode, int first, size_t count);

extern void defaultSettings();

extern void beginScene();
extern void endScene();

}

};