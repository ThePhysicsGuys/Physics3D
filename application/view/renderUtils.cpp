#include "renderUtils.h"


namespace Renderer {

	unsigned int WIREFRAME = GL_LINE;
	unsigned int FILLED = GL_FILL;
	unsigned int POINTS = GL_POINT;

	GLFWwindow* context;

	bool initGLEW() {
		return glewInit() == GLEW_OK;
	}

	bool initGLFW() {
		return glfwInit();
	}

	void terminateGLFW() {
		glfwTerminate();
	}
	
	void makeContextCurrent() {
		glfwMakeContextCurrent(context);
	}

	void createContext(int width, int height, const char* title) {
		context = glfwCreateWindow(width, height, title, nullptr, nullptr);
	}

	bool validContext() {
		return context;
	}

	void swapInterval(int interval) {
		glfwSwapInterval(interval);
	}

	void swapBuffers() {
		glfwSwapBuffers(context);
	}

	void pollEvents() {
		glfwPollEvents();
	}

	void clearDepth() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void clearColor() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void clearStencil() {
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	GLFWwindow* getContext() {
		return context;
	}

	void closeWindow() {
		glfwSetWindowShouldClose(context, GLFW_TRUE);
	}

	bool windowClosed() {
		return glfwWindowShouldClose(context);
	}

	Vec2i windowSize() {
		int width;
		int height;
		glfwGetWindowSize(context, &width, &height);
		return Vec2i(width, height);
	}

	Vec4i frameSize() {
		int left;
		int top;
		int right;
		int bottom;
		glfwGetWindowFrameSize(context, &left, &top, &right, &bottom);
		return Vec4i(left, top, right, bottom);
	}

	void viewport(Vec2i origin, Vec2i dimension) {
		glViewport(origin.x, origin.y, dimension.x, dimension.y);
	}

	inline void enableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}

	inline void disableDepthTest() {
		glDisable(GL_DEPTH_TEST);
	}

	inline void enableDepthMask() {
		glDepthMask(GL_TRUE);
	}

	inline void disableDepthMask() {
		glDepthMask(GL_FALSE);
	}

	inline void enableCulling() {
		glEnable(GL_CULL_FACE);
	}

	inline void disableCulling() {
		glDisable(GL_CULL_FACE);
	}

	inline void enableBlending() {
		glEnable(GL_BLEND);
	}

	inline void disableBlending() {
		glDisable(GL_BLEND);
	}

	inline void standardBlendFunction() {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void enableCursor() {
		glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void disableCursor() {
		glfwSetInputMode(context, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	const unsigned char* getVendor() {
		return glGetString(GL_VENDOR);
	}

	const unsigned char* getVersion() {
		return glGetString(GL_VERSION);
	}

	const unsigned char* getRenderer() {
		return glGetString(GL_RENDERER);
	}

	const unsigned char* getShaderVersion() {
		return glGetString(GL_SHADING_LANGUAGE_VERSION);
	}
}