#pragma once

#include <chrono>

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../graphics/visualShape.h"
#include "../engine/visualData.h"
#include "../engine/event/event.h"
#include "../engine/layer/layerStack.h"
#include "../engine/ecs/entity.h"
#include "camera.h"


namespace Graphics {
struct Quad;
class FrameBuffer;
class HDRFrameBuffer;
class IndexedMesh;
};

namespace Application {

class StandardInputHandler;
class PlayerWorld;

bool initGLEW();
bool initGLFW();
void terminateGLFW();

class Screen {
private:
	std::chrono::time_point<std::chrono::steady_clock> lastUpdate = std::chrono::steady_clock::now();

public:
	static std::vector<Engine::Entity*> entities;
	PlayerWorld* world;
	Vec2i dimension;

	Camera camera;
	LayerStack layerStack;
	EventHandler eventHandler;
	Properties properties;
	
	Graphics::FrameBuffer* screenFrameBuffer = nullptr;
	Graphics::Quad* quad = nullptr;

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
	void onEvent(::Event& event);
	void onRender();
	void onClose();

	bool shouldClose();
};

extern StandardInputHandler* handler;

};