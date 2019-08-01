#pragma once

#include <vector>

#include "layer/layer.h"

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

class Screen : public Layer {
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
	FrameBuffer* maskFrameBuffer = nullptr;
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

	void init() override;
	void update() override;
	void render() override;
	void close() override;

	bool shouldClose();
	int addMeshShape(const VisualShape& mesh);
};

extern StandardInputHandler* handler;
