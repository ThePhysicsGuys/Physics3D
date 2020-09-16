#pragma once

#include <chrono>

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../graphics/visualShape.h"
#include "../engine/visualData.h"
#include "../engine/event/event.h"
#include "../engine/layer/layerStack.h"
#include "camera.h"

namespace P3D::Graphics {
struct Quad;
class FrameBuffer;
class HDRFrameBuffer;
class IndexedMesh;
};

namespace P3D::Application {

class StandardInputHandler;
class PlayerWorld;

bool initGLEW();
bool initGLFW();
void terminateGLFW();

class Screen {
public:
	PlayerWorld* world;
	Vec2i dimension;

	Camera camera;
	Engine::LayerStack layerStack;
	EventHandler eventHandler;
	Util::Properties properties;
	
	Graphics::FrameBuffer* screenFrameBuffer = nullptr;
	Graphics::Quad* quad = nullptr;

	Vec3f ray;
	ExtendedPart* intersectedPart = nullptr;
	ExtendedPart* selectedPart = nullptr;
	Position intersectedPoint;
	Position selectedPoint;

	Screen();
	Screen(int width, int height, PlayerWorld* world);

	void onInit();
	void onUpdate();
	void onEvent(Engine::Event& event);
	void onRender();
	void onClose();

	bool shouldClose();
};

extern StandardInputHandler* handler;

};