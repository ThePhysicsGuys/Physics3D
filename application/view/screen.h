#pragma once

#include "../eventHandler.h"
#include "../util/properties.h"
#include "../graphics/visualShape.h"
#include "../graphics/visualData.h"
#include "../engine/event/event.h"
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
	static std::vector<IndexedMesh*> meshes;
	static std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
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
	static VisualData addMeshShape(const VisualShape& mesh);
	static VisualData registerMeshFor(const ShapeClass* shapeClass, const VisualShape& mesh);
	static VisualData registerMeshFor(const ShapeClass* shapeClass);
	static VisualData getOrCreateMeshFor(const ShapeClass* shapeClass);
};

extern StandardInputHandler* handler;
