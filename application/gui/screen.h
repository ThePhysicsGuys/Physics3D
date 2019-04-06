#pragma once

#include "../../engine/debug.h"
#include "../../engine/world.h"
#include "../../engine/math/vec2.h"
#include "../eventHandler.h"

#include "frameBuffer.h"
#include "camera.h"

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

	void renderSkybox();
	void renderPhysicals();
	template<typename T>
	void renderDebugField(const char* fieldName, T value, const char* unit);
	int fieldIndex = 0;
public:
	World* world;
	Vec2 screenSize;
	Camera camera;
	EventHandler eventHandler;

	FrameBuffer* modelFrameBuffer = nullptr;
	FrameBuffer* screenFrameBuffer = nullptr;

	// Picker
	Vec3 ray;
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

	GLFWwindow* getWindow() { return window; }

	void setWorld(World* world) { this->world = world; };

	int addMeshShape(Shape mesh);

	void toggleDebugVecType(Debug::VecType t);
};
