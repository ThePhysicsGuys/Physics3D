#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../engine/debug.h"
#include "../../engine/profiling.h"
#include "../../engine/world.h"
#include "../../engine/math/vec2.h"
#include "../eventHandler.h"
#include "../util/properties.h"

#include "frameBuffer.h"
#include "camera.h"
#include "quad.h"
#include "../worlds.h"

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
	MagnetWorld* world;
	Vec2i dimension;

	Camera camera;

	EventHandler eventHandler;
	Properties properties;

	FrameBuffer* modelFrameBuffer = nullptr;
	FrameBuffer* screenFrameBuffer = nullptr;
	FrameBuffer* blurFrameBuffer = nullptr;
	Quad* quad = nullptr;

	// Picker
	Vec3f ray;
	ExtendedPart* intersectedPart = nullptr;
	Vec3f intersectedPoint;
	ExtendedPart* selectedPart = nullptr;
	Vec3f selectedPoint;

	Screen();
	Screen(int width, int height, MagnetWorld* world);

	bool shouldClose();
	void init();
	void update();
	void refresh();
	void close();

	GLFWwindow* getWindow() { return window; }

	void setWorld(MagnetWorld* world) {
		this->world = world;
	};

	int addMeshShape(const VisualShape& mesh);
};

extern StandardInputHandler* handler;
