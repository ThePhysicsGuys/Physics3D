#pragma once

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../visualShape.h"
#include "../event/event.h"
#include "camera.h"

struct Quad;
class FrameBuffer;
class HDRFrameBuffer;
class StandardInputHandler;
class IndexedMesh;
class PlayerWorld;

bool initGLEW();
bool initGLFW();
void terminateGLFW();

class Screen {
public:
	std::vector<IndexedMesh*> meshes;
	PlayerWorld* world;
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
	Screen(int width, int height, PlayerWorld* world);

	void onInit();
	void onUpdate();
	void onEvent(Event& event);
	void onRender();
	void onClose();

	bool shouldClose();
	int addMeshShape(const VisualShape& mesh);
};

extern StandardInputHandler* handler;
