#include "core.h"

#include "glfwUtils.h"

#include <GLFW/glfw3.h>

namespace Graphics {

namespace GLFW {

namespace Cursor {
	int ARROW = GLFW_ARROW_CURSOR;
	int IBEAM = GLFW_IBEAM_CURSOR;
	int CROSSHAIR = GLFW_CROSSHAIR_CURSOR;
	int HAND = GLFW_HAND_CURSOR;
	int HRESIZE = GLFW_HRESIZE_CURSOR;
	int VRESIZE = GLFW_VRESIZE_CURSOR;
}

GLFWwindow* currentContext;
GLFWcursor* currentCursor;
int currentCursorType;

bool init() {
	return glfwInit();
}

void terminate() {
	glfwTerminate();
}

void setCurrentContext(GLFWwindow* context) {
	currentContext = context;
	glfwMakeContextCurrent(currentContext);
}

GLFWwindow* createContext(int width, int height, const char* title) {
	GLFWwindow* context = glfwCreateWindow(width, height, title, nullptr, nullptr);

	return context;
}

bool validContext(GLFWwindow* context) {
	return context != nullptr;
}

GLFWwindow* getCurrentContext() {
	return currentContext;
}

void swapInterval(int interval) {
	glfwSwapInterval(interval);
}

void swapBuffers() {
	glfwSwapBuffers(currentContext);
}

void pollEvents() {
	glfwPollEvents();
}

void closeWindow() {
	glfwSetWindowShouldClose(currentContext, GLFW_TRUE);
}

bool isWindowClosed() {
	return glfwWindowShouldClose(currentContext);
}

void setMultisampleSamples(int samples) {
	glfwWindowHint(GLFW_SAMPLES, samples);
};

Vec2i getWindowSize() {
	int width;
	int height;
	glfwGetWindowSize(currentContext, &width, &height);
	return Vec2i(width, height);
}

Vec4i getFrameSize() {
	int left;
	int top;
	int right;
	int bottom;
	glfwGetWindowFrameSize(currentContext, &left, &top, &right, &bottom);
	return Vec4i(left, top, right, bottom);
}

void enableCursor() {
	glfwSetInputMode(currentContext, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void disableCursor() {
	glfwSetInputMode(currentContext, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void setCursor(int type) {
	if (currentCursorType == type)
		return;

	if (currentCursor)
		glfwDestroyCursor(currentCursor);
	
	currentCursorType = type;
	currentCursor = glfwCreateStandardCursor(type);

	glfwSetCursor(currentContext, currentCursor);
}

}

};