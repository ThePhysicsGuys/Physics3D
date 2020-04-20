#pragma once

struct GLFWwindow;

namespace Graphics {

namespace Renderer {

// GLFW binding

extern bool initGLFW();
extern void terminateGLFW();

extern GLFWwindow* getGLFWContext();
extern void createGLFWContext(int width, int height, const char* title);
extern void makeGLFWContextCurrent();
extern bool validGLFWContext();

extern void swapGLFWInterval(int interval);
extern void swapGLFWBuffers();
extern void pollGLFWEvents();

extern void closeGLFWWindow();
extern bool isGLFWWindowClosed();

extern Vec2i getGLFWWindowSize();
extern Vec4i getGLFWFrameSize();

extern void enableGLFWCursor();
extern void disableGLFWCursor();

extern void setGLFWMultisampleSamples(int samples);
}

};