#pragma once

#include "../../engine/world.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool initGLFW();
bool initGLEW();

void terminateGL();

class Screen {
private:
	GLFWwindow* window;
	World* w;
public:
	Screen() {};
	Screen(int width, int height, World* world);

	bool shouldClose();
	void init();
	void update();
	void refresh();
	void close();

	void makeCurrent();

	void setWorld(World* w) { this->w = w; };
};
