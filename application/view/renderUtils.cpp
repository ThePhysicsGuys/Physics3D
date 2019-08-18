#include "renderUtils.h"


namespace Renderer {

	unsigned int WIREFRAME = GL_LINE;
	unsigned int FILL = GL_FILL;
	unsigned int POINT = GL_POINT;

	unsigned int STATIC_DRAW = GL_STATIC_DRAW;
	unsigned int STREAM_DRAW = GL_STREAM_DRAW;
	unsigned int DYNAMIC_DRAW = GL_DYNAMIC_DRAW;

	unsigned int PATCHES = GL_PATCHES;
	unsigned int QUADS = GL_QUADS;
	unsigned int TRIANGLES = GL_TRIANGLES;
	unsigned int LINES = GL_LINES;
	unsigned int POINTS = GL_POINTS;

	unsigned int UINT = GL_UNSIGNED_INT;

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

	// GLEW binding

	bool initGLEW() {
		return glewInit() == GLEW_OK;
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

	void lineStipple(int factor, short pattern) {
		glLineStipple(factor, pattern);
	}

	void lineWidth(float size) {
		glLineWidth(size);
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

	inline void enableMultisampling() {
		glEnable(GL_MULTISAMPLE);
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

	inline void drawElements(unsigned int mode, size_t count, unsigned int type, const void * offset) {
		glDrawElements(mode, count, type, offset);
	}

	inline void drawArrays(unsigned int mode, int first, size_t count) {
		glDrawArrays(mode, first, count);
	}
}