#pragma once

#include "../../engine/world.h"

#include <GLFW/glfw3.h>

bool initGL();
void terminateGL();

class Screen {
private:
	GLFWwindow* window;
	World* w;
public:
	Screen(int width, int height, World* world);
	~Screen();

	bool shouldClose();
	void refresh();

	void makeCurrent();

	void setWorld(World* w) { this->w = w; };
};
