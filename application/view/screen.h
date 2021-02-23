#pragma once

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../graphics/visualShape.h"
#include "../engine/event/event.h"
#include "../engine/layer/layerStack.h"
#include "../engine/ecs/registry.h"
#include "picker/selectionContext.h"
#include "camera.h"
#include "../../engine/tool/toolManager.h"

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
	Vec2i dimension;

	Camera camera;
	Engine::LayerStack layerStack;
	EventHandler eventHandler;
	Util::Properties properties;
	
	Graphics::FrameBuffer* screenFrameBuffer = nullptr;
	Graphics::Quad* quad = nullptr;

	Engine::Registry64::entity_type intersectedEntity = 0;
	Engine::Registry64::entity_type selectedEntity = 0;
	ExtendedPart* intersectedPart = nullptr;
	ExtendedPart* selectedPart = nullptr;
	Position intersectedPoint;
	Position selectedPoint;
	SelectionContext selectionContext;

	Screen();
	Screen(int width, int height, PlayerWorld* world);

	void onInit(bool quickBoot);
	void onUpdate();
	void onEvent(Engine::Event& event);
	void onRender();
	void onClose();

	bool shouldClose();
};

extern StandardInputHandler* handler;

};