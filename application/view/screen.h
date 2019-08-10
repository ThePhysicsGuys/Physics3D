#pragma once

#include <vector>

#include "../engine/math/vec2.h"
#include "../engine/math/vec3.h"

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../visualShape.h"
#include "camera.h"

struct Quad;
class FrameBuffer;
class HDRFrameBuffer;
class StandardInputHandler;
class IndexedMesh;
class MagnetWorld;

bool initGLEW();
bool initGLFW();
void terminateGLFW();

class Screen {
public:
	std::vector<IndexedMesh*> meshes;
	MagnetWorld* world;
	Vec2i dimension;

	Camera camera;

	EventHandler eventHandler;
	Properties properties;

	FrameBuffer* screenFrameBuffer = nullptr;
	FrameBuffer* blurFrameBuffer = nullptr;
	Quad* quad = nullptr;

	// Picker
	Vec3f ray;
	ExtendedPart* intersectedPart = nullptr;
	Position intersectedPoint;
	ExtendedPart* selectedPart = nullptr;
	Position selectedPoint;

	Screen();
	Screen(int width, int height, MagnetWorld* world);

	void init();
	void update();
	void render();
	void close();

	bool shouldClose();
	int addMeshShape(const VisualShape& mesh);
};

extern StandardInputHandler* handler;
