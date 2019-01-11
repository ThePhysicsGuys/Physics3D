#pragma once

#include "../../engine/world.h"
#include "../../engine/math/vec2.h"
#include "../eventHandler.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "indexedMesh.h"

bool initGLFW();
bool initGLEW();

void terminateGL();

class Screen {
private:
	GLFWwindow* window;
	std::vector<IndexedMesh*> meshes;
public:
	World* world;
	Vec2 screenSize;
	EventHandler eventHandler;

	Physical* intersectedPhysical = nullptr;
	Vec3 intersectedPoint;
	Physical* selectedPhysical = nullptr;
	Vec3 selectedPoint;

	Screen() {};
	Screen(int width, int height, World* world);

	bool shouldClose();
	void init();
	void update();
	void refresh();
	void close();

	void makeCurrent();

	void setWorld(World* world) { this->world = world; };

	int addMeshShape(Shape mesh);
};
