#include "core.h"

#include "application.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "view/screen.h"

#include "../graphics/resources.h"
#include "../graphics/texture.h"
#include "../graphics/material.h"
#include "../graphics/debug/debug.h"
#include "../graphics/debug/visualDebug.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/basicShapes.h"
#include "../physics/math/mathUtil.h"
#include "../physics/part.h"
#include "../physics/world.h"
#include "../physics/misc/gravityForce.h"
#include "../physics/physicsProfiler.h"

#include "worlds.h"
#include "tickerThread.h"
#include "worldBuilder.h"

#include "io/export.h"
#include "io/import.h"

#include "../util/resource/resourceLoader.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/resource/textureResource.h"
#include "../engine/io/import.h"

#define TICKS_PER_SECOND 120.0
#define TICK_SKIP_TIME std::chrono::milliseconds(10)


TickerThread physicsThread;
PlayerWorld world(1 / TICKS_PER_SECOND);
Screen screen;

void init();

int main(void) {
	init();

	Log::info("Started rendering");
	while (!screen.shouldClose()) {
		graphicsMeasure.mark(GraphicsProcess::UPDATE);
		screen.onUpdate();
		screen.onRender();
		graphicsMeasure.end();
	}

	stop(0);
}

void setupPhysics();
void registerShapes();
void setupWorld();
void setupScreen();
void setupDebug();


void init() {
	setupScreen();
	registerShapes();
	setupWorld();
	setupPhysics();
	setupDebug();
}

void setupScreen() {
	Log::info("Initializing GLFW");
	if (!initGLFW()) {
		Log::error("GLFW not initialised");
		std::cin.get();
		stop(-1);
	}

	screen = Screen(800, 640, &world);

	Log::info("Initializing GLEW");
	if (!initGLEW()) {
		Log::error("GLEW not initialised");
		std::cin.get();
		stop(-1);
	}

	Log::info("Initializing screen");
	screen.onInit();
}

void registerShapes() {
	Polyhedron sphere(Library::createSphere(1.0, 3));
	VisualShape sphereShape = VisualShape(sphere);
	Vec3f* normalBuf = new Vec3f[sphereShape.vertexCount];
	sphereShape.computeNormals(normalBuf);
	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuf);
	
	screen.registerMeshFor(Sphere(1.0).baseShape, sphereShape);

	screen.registerMeshFor(Box(2.0, 2.0, 2.0).baseShape);
}



void setupWorld() {
	Log::info("Initializing world");

	world.addExternalForce(new ExternalGravity(Vec3(0, -10.0, 0.0)));

	// WorldBuilder init
	WorldBuilder::init();

	// Part factories
	WorldBuilder::SpiderFactory spiderFactories[]{ {0.5, 3},{0.5, 4},{0.5, 5},{0.5, 6} };
	Shape triangle(Library::trianglePyramid);

	WorldBuilder::buildFloorAndWalls(50.0, 50.0, 7.0);

	world.addPart(new ExtendedPart(Sphere(2.0), GlobalCFrame(10, 3, 10), {1.0, 0.3, 0.7}, "SphereThing"));

	ExtendedPart* conveyor = new ExtendedPart(Box(1.0, 0.3, 50.0), GlobalCFrame(10.0, 0.65, 0.0), { 2.0, 1.0, 0.3 });

	conveyor->properties.conveyorEffect = Vec3(0, 0, 2.0);
	world.addTerrainPart(conveyor);

	world.addPart(new ExtendedPart(Box(0.2, 0.2, 0.2), GlobalCFrame(10, 1.0, 0.0), { 1.0, 0.2, 0.3 }, "TinyCube"));

	// hollow box
	WorldBuilder::HollowBoxParts parts = WorldBuilder::buildHollowBox(Bounds(Position(12.0, 3.0, 14.0), Position(20.0, 8.0, 20.0)), 0.3);

	parts.front->material.ambient = Vec4f(0.4, 0.6, 1.0, 0.3);
	parts.back->material.ambient = Vec4f(0.4, 0.6, 1.0, 0.3);

	// Rotating walls
	ExtendedPart* rotatingWall = new ExtendedPart(Box(5.0, 3.0, 0.5), GlobalCFrame(Position(-12.0, 1.7, 0.0)), {1.0, 1.0, 0.7});
	ExtendedPart* rotatingWall2 = new ExtendedPart(Box(5.0, 3.0, 0.5), GlobalCFrame(Position(-12.0, 1.7, 5.0)), {1.0, 1.0, 0.7});
	world.addPart(rotatingWall, true);
	world.addPart(rotatingWall2, true);
	rotatingWall->parent->angularVelocity = Vec3(0, -0.7, 0);
	rotatingWall2->parent->angularVelocity = Vec3(0, 0.7, 0);

	// Many many parts
	for (int i = 0; i < 3; i++) {
		ExtendedPart* newCube = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(fRand(-10.0, 0.0), fRand(1.0, 1.0), fRand(-10.0, 0.0)), {1.0, 0.2, 0.7});
		world.addPart(newCube);
	}

	WorldBuilder::buildCar(GlobalCFrame(5.0, 1.0, 5.0));
	
	int minX = 0;
	int maxX = 2;
	int minY = 0;
	int maxY = 2;
	int minZ = 0;
	int maxZ = 2;

	GlobalCFrame rootFrame(Position(0.0, 15.0, 0.0), fromEulerAngles(3.1415 / 4, 3.1415 / 4, 0.0));

	for (double x = minX; x < maxX; x += 1.00001) {
		for (double y = minY; y < maxY; y += 1.00001) {
			for (double z = minZ; z < maxZ; z += 1.00001) {
				ExtendedPart* newCube = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(Position(x - 5, y + 10, z - 5)), { 1.0, 1.0, 0.0 });
				newCube->material.ambient = Vec4f((x-minX)/(maxX-minX), (y-minY)/(maxY-minY), (z-minZ)/(maxZ-minZ), 1.0f);
				world.addPart(newCube);
				world.addPart(new ExtendedPart(Sphere(0.5), GlobalCFrame(Position(x + 5, y + 1, z - 5)), { 1.0, 0.2, 0.5 }));
				spiderFactories[rand() & 0x00000003].buildSpider(GlobalCFrame(Position(x+y*0.1, y+1, z)));
				world.addPart(new ExtendedPart(triangle, GlobalCFrame(Position(x - 20, y + 1, z + 20)), { 1.0, 0.2, 0.5 }));
			}
		}
	}

	WorldBuilder::buildTerrain(500.0, 500.0);
	world.optimizeTerrain();

	// Player
	screen.camera.attachment = new ExtendedPart(Library::createPrism(50, 1.0, 2.0), GlobalCFrame(), { 1.0, 5.0, 0.0 }, "Player");
	screen.camera.attachment->properties.conveyorEffect = Vec3(1.0, 0.0, 1.0);

	if (!world.isValid()) {
		throw "World not valid!";
	}
}

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, []() {
		physicsMeasure.mark(PhysicsProcess::OTHER);

		AppDebug::logTickStart();
		world.tick();
		AppDebug::logTickEnd();

		physicsMeasure.end();

		GJKCollidesIterationStatistics.nextTally();
		GJKNoCollidesIterationStatistics.nextTally();
		EPAIterationStatistics.nextTally();
	});
}

void setupDebug() {
	AppDebug::setupDebugHooks();
}

void stop(int returnCode) {
	Log::info("Closing physics");
	physicsThread.stop();

	Log::info("Closing screen");
	screen.onClose();

	exit(returnCode);
}


// Ticks

bool paused = true;

void togglePause() {
	if(paused) {
		unpause();
	} else {
		pause();
	}
}

void pause() {
	physicsThread.stop();
	paused = true;
}

void unpause() {
	physicsThread.start();
	paused = false;
}

bool isPaused() {
	return paused;
}

void setSpeed(double newSpeed) {
	physicsThread.setSpeed(newSpeed);
}

double getSpeed() {
	return physicsThread.getSpeed();
}

void runTick() {
	physicsThread.runTick();
}


// Flying

void toggleFlying() {
	world.asyncModification([]() {
		if (screen.camera.flying) {
			screen.camera.flying = false;
			screen.camera.attachment->setCFrame(GlobalCFrame(screen.camera.cframe.getPosition()));
			screen.world->addPart(screen.camera.attachment);
			screen.camera.attachment->parent->momentResponse = SymmetricMat3::ZEROS();
		} else {
			screen.world->removePart(screen.camera.attachment);
			screen.camera.flying = true;
		}
	});
}
