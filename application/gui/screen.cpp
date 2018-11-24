#include "screen.h"

#include <GLFW/glfw3.h>

#include <stdlib.h>

World* curWorld = NULL;

bool initGL() {
	/* Initialize the library */
	return glfwInit() != 0;
}

void terminateGL() {
	glfwTerminate();
}

Screen::Screen(int width, int height, World* w) {
	setWorld(w);

	/* Create a windowed mode window and its OpenGL context */
	this->window = glfwCreateWindow(width, height, "Physics3D", NULL, NULL);
	if (!this->window) {
		glfwTerminate();
		exit(-1);
	}

	/* Make the window's context current */
	makeCurrent();
}

Screen::~Screen() {

}

void Screen::makeCurrent() {
	glfwMakeContextCurrent(this->window);
}

void Screen::refresh() {

	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Swap front and back buffers */
	glfwSwapBuffers(this->window);

	/* Poll for and process events */
	glfwPollEvents();
}

bool Screen::shouldClose() {
	return glfwWindowShouldClose(window);
}
