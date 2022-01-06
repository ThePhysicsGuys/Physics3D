#pragma once

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../graphics/visualShape.h"
#include "../engine/event/event.h"
#include "../engine/layer/layerStack.h"
#include "../engine/ecs/registry.h"
#include "camera.h"

namespace P3D {
class UpgradeableMutex;
};

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
	Engine::Registry64 registry;

	PlayerWorld* world;
	UpgradeableMutex* worldMutex;
	Vec2i dimension;

	Camera camera;
	Engine::LayerStack layerStack;
	EventHandler eventHandler;
	Util::Properties properties;	
	
	SRef<Graphics::FrameBuffer> screenFrameBuffer = nullptr;
	Graphics::Quad* quad = nullptr;

	ExtendedPart* selectedPart = nullptr;

	Screen();
	Screen(int width, int height, PlayerWorld* world, UpgradeableMutex* worldMutex);

	void onInit(bool quickBoot);
	void onUpdate();
	void onEvent(Engine::Event& event);
	void onRender();
	void onClose();

	bool shouldClose();
};

extern StandardInputHandler* handler;

};