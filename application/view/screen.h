#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../engine/profiling.h"
#include "../engine/math/vec2.h"
#include "../eventHandler.h"
#include "../util/properties.h"
#include "../engine/debug.h"
#include "../engine/datastructures/buffers.h"

#include "buffers/frameBuffer.h"
#include "camera.h"
#include "mesh/primitive.h"

class StandardInputHandler;
class MagnetWorld;

#include "mesh/indexedMesh.h"
#include "../standardInputHandler.h"

bool initGLEW();
bool initGLFW();
void terminateGLFW();

class Screen {
private:
	void renderSkybox();
	void renderPhysicals();
	void renderDebug();
	void renderPies();
public:
	std::vector<IndexedMesh*> meshes;
	MagnetWorld* world;
	Vec2i dimension;

	Camera camera;

	EventHandler eventHandler;
	Properties properties;

	HDRFrameBuffer* modelFrameBuffer = nullptr;
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

	void setWorld(MagnetWorld* world) {
		this->world = world;
	};

	int addMeshShape(const VisualShape& mesh);
};

extern StandardInputHandler* handler;
