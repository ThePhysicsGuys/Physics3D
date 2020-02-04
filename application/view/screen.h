#pragma once

#include <chrono>

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../graphics/visualShape.h"
#include "../graphics/visualData.h"
#include "../engine/event/event.h"
#include "../engine/layer/layerStack.h"
#include "../engine/ecs/entity.h"
#include "camera.h"

struct Quad;
class FrameBuffer;
class HDRFrameBuffer;
class IndexedMesh;

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
	static std::vector<IndexedMesh*> meshes;
	static std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
	PlayerWorld* world;
	Vec2i dimension;

	Camera camera;
	LayerStack layerStack;
	EventHandler eventHandler;
	Properties properties;

	FrameBuffer* screenFrameBuffer = nullptr;
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
	void onEvent(::Event& event);
	void onRender();
	void onClose();

	bool shouldClose();
	static VisualData addMeshShape(const VisualShape& mesh);
	static VisualData registerMeshFor(const ShapeClass* shapeClass, const VisualShape& mesh);
	static VisualData registerMeshFor(const ShapeClass* shapeClass);
	static VisualData getOrCreateMeshFor(const ShapeClass* shapeClass);
};

extern StandardInputHandler* handler;

};