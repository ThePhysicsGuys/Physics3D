#include "application.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "view/screen.h"
#include "view/texture.h"
#include "view/debug/visualDebug.h"
#include "view/material.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/convexShapeBuilder.h"
#include "../engine/math/mathUtil.h"
#include "../engine/part.h"
#include "../engine/world.h"
#include "../engine/physicsProfiler.h"

#include "../util/log.h"

#include "debug.h"
#include "worlds.h"
#include "tickerThread.h"
#include "resourceManager.h"
#include "worldBuilder.h"

#include "io/export.h"
#include "io/import.h"

#define TICKS_PER_SECOND 120.0
#define TICK_SKIP_TIME std::chrono::milliseconds(3000)


TickerThread physicsThread;
GravityWorld world(Vec3(0.0, -10.0, 0.0));
Screen screen;


int main(void) {
	init();

	Log::info("Started rendering");
	while (!screen.shouldClose()) {
		graphicsMeasure.mark(GraphicsProcess::UPDATE);
		screen.update();
		screen.render();
		graphicsMeasure.end();
	}

	stop(0);
}

void init() {
	setupScreen();
	setupPhysics();
	setupWorld();
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
	screen.init();
}

void setupWorld() {
	Log::info("Initializing world");

	// WorldBuilder init
	WorldBuilder::init();

	// Sphere shape
	VisualShape sphereShape = OBJImport::load((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL)));
	Vec3f* normalBuf = new Vec3f[sphereShape.vertexCount];
	sphereShape.computeNormals(normalBuf);
	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuf);

	// Part factories
	WorldBuilder::SpiderFactory spiderFactories[]{ {0.5, 3},{0.5, 4},{0.5, 5},{0.5, 6} };
	PartFactory legFactory = PartFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");
	PartFactory cubeFactory(createCube(0.2), screen, "Cube");
	PartFactory sphereFactory(sphereShape, screen, "Sphere");
	PartFactory triangleFactory(triangleShape, screen, "Triangle");

	// Floor
	Vec2 floorSize(30.0, 30.0);
	double wallHeight = 7.0;
	Material floorMaterial = Material(load("../res/textures/floor/floor_color.jpg"));
	ExtendedPart* floorExtendedPart = createUniquePart(screen, BoundingBox(floorSize.x, 1.0, floorSize.y).toShape(), CFrame(Vec3(0.0, -0.15, 0.0)), 2.0, 1.0);
	floorExtendedPart->material = floorMaterial;
	world.addPart(floorExtendedPart, true);

	// Walls
	/*PartFactory xWallFactory(BoundingBox(0.7, wallHeight, floorSize.y).toShape(), screen, "xWall");
	PartFactory zWallFactory(BoundingBox(floorSize.x, wallHeight, 0.7).toShape(), screen, "zWall");
	world.addPart(xWallFactory.produce(CFrame(Vec3(floorSize.x / 2, wallHeight / 2, 0.0)), 0.2, 1.0), true);
	world.addPart(zWallFactory.produce(CFrame(Vec3(0.0, wallHeight / 2, floorSize.y / 2)), 0.2, 1.0), true);
	world.addPart(xWallFactory.produce(CFrame(Vec3(-floorSize.x / 2, wallHeight / 2, 0.0)), 0.2, 1.0), true);
	world.addPart(zWallFactory.produce(CFrame(Vec3(0.0, wallHeight / 2, -floorSize.y / 2)), 0.2, 1.0), true);*/

	// Rotating walls
	PartFactory rotatingWallFactory(BoundingBox(5.0, 3.0, 0.5).toShape(), screen, "rotatingWall");
	ExtendedPart* rotatingWall = rotatingWallFactory.produce(CFrame(Vec3(-12, 1.5, 0.0)), 0.2, 1.0);
	ExtendedPart* rotatingWall2 = rotatingWallFactory.produce(CFrame(Vec3(-12, 1.5, 5.0)), 0.2, 1.0);
	//rotatingWall->parent->angularVelocity = Vec3(0, -0.7, 0);
	//rotatingWall2->parent->angularVelocity = Vec3(0, 0.7, 0);
	//world.add(rotatingWall, true);
	//world.add(rotatingWall2, true);

	// Many many parts
	/*for (int i = 0; i < 10000; i++) {
		ExtendedPart* newCube = cubeFactory.produce(CFrame(Vec3(fRand(-10.0, 10.0), fRand(0.0, 20.0), fRand(-10.0, 10.0))), 1.0, 0.2);
		world.addPart(newCube);
	}*/


	int minX = -5;
	int maxX = 5;
	int minY = 0;
	int maxY = 5;
	int minZ = -5;
	int maxZ = 5;

	for (double x = minX; x < maxX; x += 1.01) {
		for (double y = minY; y < maxY; y += 1.01) {
			for (double z = minZ; z < maxZ; z += 1.01) {
				//ExtendedPart* newCube = cubeFactory.produce(CFrame(Vec3(x, y + 1, z)), 1.0, 0.2);
				//newCube->material.ambient = Vec4f((x-minX)/(maxX-minX), (y-minY)/(maxY-minY), (z-minZ)/(maxZ-minZ), 1.0f);
				//world.addPart(newCube);
				//world.addPart(sphereFactory.produce(CFrame(Vec3(x, y + 1, z - 5)), 1.0, 0.2));
				//spiderFactories[rand() & 0x00000003].buildSpider(CFrame(Vec3(x+y*0.1, y+1, z)));
				world.addPart(triangleFactory.produce(CFrame(Vec3(x, y + 1, z + 5)), 1.0, 0.2));
			}
		}
	}


	// Player
	screen.camera.attachment = sphereFactory.produce(CFrame(), 1.0, 0.2);
	screen.camera.attachment->properties.friction = 0.5;
	screen.camera.attachment->drawMeshId = -1;

	if (!world.isValid()) {
		throw "World not valid!";
	}
}

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, []() {
		physicsMeasure.mark(PhysicsProcess::OTHER);

		AppDebug::logTickStart();
		world.tick(1 / physicsThread.getTPS());
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
	screen.close();

	exit(returnCode);
}


// Ticks

bool paused = true;

void togglePause() {
	if (paused)
		unpause();
	else
		pause();
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
	if(screen.camera.flying) {
		screen.world->addPart(screen.camera.attachment);
		screen.camera.flying = false;
	} else {
		screen.world->removePart(screen.camera.attachment);
		screen.camera.flying = true;
	}
}
