#pragma once

#include "../../engine/debug.h"
#include "../../engine/profiling.h"
#include "../../engine/world.h"
#include "../../engine/math/vec2.h"
#include "../eventHandler.h"
#include "../util/properties.h"

#include "frameBuffer.h"
#include "camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class StandardInputHandler;

#include "indexedMesh.h"
#include "../standardInputHandler.h"

bool initGLFW();
bool initGLEW();

void terminateGL();

class Screen {
private:
	GLFWwindow* window;

	void renderSkybox();
	void renderPhysicals();
public:
	std::vector<IndexedMesh*> meshes;
	World<ExtendedPart>* world;
	Vec2 dimension;
	double aspect;
	Camera camera;
	EventHandler eventHandler;
	Properties properties;

	FrameBuffer* modelFrameBuffer = nullptr;
	FrameBuffer* screenFrameBuffer = nullptr;

	// Picker
	Vec3 ray;
	ExtendedPart* intersectedPart = nullptr;
	Vec3 intersectedPoint;
	ExtendedPart* selectedPart = nullptr;
	Vec3 selectedPoint;

	Screen();
	Screen(int width, int height, World<ExtendedPart>* world);

	bool shouldClose();
	void init();
	void update();
	void refresh();
	void close();

	GLFWwindow* getWindow() { return window; }

	void setWorld(World<ExtendedPart>* world) { 
		this->world = world;
	};

	int addMeshShape(Shape mesh);
};

extern StandardInputHandler* handler;
