#include "core.h"

#include "renderUtils.h"

namespace Graphics {

namespace Renderer {

// GLFW binding

GLFWwindow* GLFWContext;

bool initGLFW() {
	return glfwInit();
}

void terminateGLFW() {
	glfwTerminate();
}

void makeGLFWContextCurrent() {
	glfwMakeContextCurrent(GLFWContext);
}

void createGLFWContext(int width, int height, const char* title) {
	GLFWContext = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

bool validGLFWContext() {
	return GLFWContext;
}

GLFWwindow* getGLFWContext() {
	return GLFWContext;
}

void swapGLFWInterval(int interval) {
	glfwSwapInterval(interval);
}

void swapGLFWBuffers() {
	glfwSwapBuffers(GLFWContext);
}

void pollGLFWEvents() {
	glfwPollEvents();
}

void closeGLFWWindow() {
	glfwSetWindowShouldClose(GLFWContext, GLFW_TRUE);
}

bool isGLFWWindowClosed() {
	return glfwWindowShouldClose(GLFWContext);
}

void setGLFWMultisampleSamples(int samples) {
	glfwWindowHint(GLFW_SAMPLES, samples);
};

Vec2i getGLFWWindowSize() {
	int width;
	int height;
	glfwGetWindowSize(GLFWContext, &width, &height);
	return Vec2i(width, height);
}

Vec4i getGLFWFrameSize() {
	int left;
	int top;
	int right;
	int bottom;
	glfwGetWindowFrameSize(GLFWContext, &left, &top, &right, &bottom);
	return Vec4i(left, top, right, bottom);
}

void enableGLFWCursor() {
	glfwSetInputMode(GLFWContext, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void disableGLFWCursor() {
	glfwSetInputMode(GLFWContext, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

}

};