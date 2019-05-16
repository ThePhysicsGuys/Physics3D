#include "application.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>

#include "view/screen.h"
#include "view/visualDebug.h"
#include "view/material.h"
#include "view/loader.h"

#include "../engine/math/mathUtil.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/managedShape.h"
#include "../engine/geometry/convexShapeBuilder.h"

#include "../engine/part.h"
#include "../engine/world.h"
#include "../engine/physicsProfiler.h"
#include "../engine/engineException.h"

#include "../util/log.h"

#include "debug.h"
#include "worlds.h"
#include "tickerThread.h"
#include "resourceManager.h"
#include "objectLibrary.h"
#include "extendedPart.h"
#include "partFactory.h"


#define _USE_MATH_DEFINES
#include "math.h"

#define TICKS_PER_SECOND 500.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)

Screen screen;
GravityWorld world(Vec3(0.0, -10.0, 0.0));

TickerThread physicsThread;

ExtendedPart* player;
bool flying = true;

void init();
void setupPhysics();

Vec3 dominoBuf[8];
Shape dominoShape = BoundingBox{-0.1, -0.7, -0.3, 0.1, 0.7, 0.3}.toShape(dominoBuf);
PartFactory dominoFactory;

void createDominoAt(Vec3 pos, Mat3 rotation) {
	ExtendedPart* domino = dominoFactory.produce(CFrame(pos, rotation), 1, 0.1);
	world.addObject(domino);
}

void makeDominoStrip(int dominoCount) {
	for(int i = 0; i < dominoCount; i++) {
		createDominoAt(Vec3(i*0.5, 0.7, 1.3), Mat3());
	}
}

void makeDominoTower(int floors, int circumference, Vec3 origin) {
	double radius = circumference / 4.4;
	Mat3 sideways = fromEulerAngles(M_PI / 2, 0.0, 0.0);
	for(int floor = 0; floor < floors; floor++) {
		for(int j = 0; j < circumference; j++) {
			double angle = (2 * M_PI * (j + (floor % 2) / 2.0)) / circumference;
			Vec3 pos = Vec3(std::cos(angle)*radius, floor * 0.7 + 0.30, std::sin(angle) * radius);
			createDominoAt(pos + origin, rotY(-angle) * sideways);
		}
	}
}

inline int furthestIndexInDirection(Vec3* vertices, int vertexCount, Vec3 direction) {
	double bestDot = vertices[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		double newD = vertices[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

int main(void) {
	init();

	dominoFactory = PartFactory(dominoShape, screen, "domino");
	
	int* builderRemovalBuffer = new int[1000];
	EdgePiece* builderAddingBuffer = new EdgePiece[1000];

	world.addObject(createUniquePart(screen, dominoShape, CFrame(Vec3(1.5, 0.7, -7.3), fromEulerAngles(0.0, 0.2, 0.0)), 2.0, 0.7));

	Vec2 floorSize(40.0, 80.0);
	double wallHeight = 3.0;

	Material floorMaterial = Material(load("../res/textures/floor/floor_color.jpg"));

	ExtendedPart* floorExtendedPart = createUniquePart(screen, BoundingBox(floorSize.x, 0.3, floorSize.y).toShape(new Vec3[8]), CFrame(Vec3(0.0, -0.15, 0.0)), 0.2, 1.0);
	floorExtendedPart->material = floorMaterial;
	world.addObject(floorExtendedPart, true);

	PartFactory xWallFactory(BoundingBox(0.2, wallHeight, floorSize.y).toShape(new Vec3[8]), screen, "xWall");
	PartFactory zWallFactory(BoundingBox(floorSize.x, wallHeight, 0.2).toShape(new Vec3[8]), screen, "yWall");

	world.addObject(xWallFactory.produce(CFrame(Vec3(floorSize.x / 2, wallHeight / 2, 0.0)), 0.2, 1.0), true);
	world.addObject(zWallFactory.produce(CFrame(Vec3(0.0, wallHeight / 2, floorSize.y / 2)), 0.2, 1.0), true);
	world.addObject(xWallFactory.produce(CFrame(Vec3(-floorSize.x / 2, wallHeight / 2, 0.0)), 0.2, 1.0), true);
	world.addObject(zWallFactory.produce(CFrame(Vec3(0.0, wallHeight / 2, -floorSize.y / 2)), 0.2, 1.0), true);

	ExtendedPart* ramp = createUniquePart(screen, BoundingBox(10.0, 0.17, 3.0).toShape(new Vec3[8]), CFrame(Vec3(12.0, 1.5, 0.0), fromEulerAngles(M_PI / 2 * 0.2, M_PI/2, 0.0)), 0.2, 1.0);
	world.addObject(ramp, true);

	PartFactory rotatingWallFactory(BoundingBox(5.0, 3.0, 0.5).toShape(new Vec3[8]), screen, "rotatingWall");
	ExtendedPart* rotatingWall = rotatingWallFactory.produce(CFrame(Vec3(-12, 1.5, 0.0)), 0.2, 1.0);
	world.add(rotatingWall, true);
	rotatingWall->parent->angularVelocity = Vec3(0, -0.7, 0);

	ExtendedPart* rotatingWall2 = rotatingWallFactory.produce(CFrame(Vec3(-12, 1.5, 5.0)), 0.2, 1.0);
	world.add(rotatingWall2, true);
	rotatingWall2->parent->angularVelocity = Vec3(0, 0.7, 0);

	// makeDominoStrip(20);
	// makeDominoTower(20, 10, Vec3(-4.0, 0.0, -4.0));

	Vec3 newIcosaVerts[30];
	Triangle newIcosaTriangles[40];
	TriangleNeighbors icosaNeighBuf[40];

	ConvexShapeBuilder icosaBuilder(icosahedron, newIcosaVerts, newIcosaTriangles, icosaNeighBuf, builderRemovalBuffer, builderAddingBuffer);

	if(!icosaBuilder.toIndexedShape().isValid()) {
		throw "BAD";
	}

	icosaBuilder.addPoint(Vec3(0, 1.1, 0));
	icosaBuilder.addPoint(Vec3(0, -1.1, 0));
	icosaBuilder.addPoint(Vec3(1.1, 0, 0));
	icosaBuilder.addPoint(Vec3(-1.1, 0, 0));
	icosaBuilder.addPoint(Vec3(0, 0, 1.1));
	icosaBuilder.addPoint(Vec3(0, 0, -1.1));

	if(!icosaBuilder.toIndexedShape().isValid()) {
		throw "BAD";
	}

	Shape newIcosa = icosaBuilder.toShape();
	ExtendedPart* constructedIcosa = createUniquePart(screen, newIcosa, CFrame(Vec3(10, 0, -10)), 2.0, 0.7);

	Vec3 verts[10]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	ConvexShapeBuilder builder(verts, triangles, 4, 4, neighBuf, builderRemovalBuffer, builderAddingBuffer);

	builder.addPoint(Vec3(0.4, 0.4, 0.4), 3);

	builder.addPoint(Vec3(-0.4, 1, -0.4));

	builder.addPoint(Vec3(-0.8, 0.5, -0.8));

	builder.addPoint(Vec3(-0.9, 0.6, -0.9));

	Shape constructedShape = builder.toShape();

	ExtendedPart* constructedExtendedPart = createUniquePart(screen, constructedShape, CFrame(Vec3(0.0, 2.0, -5.0)), 2.0, 0.7);
	world.addObject(constructedExtendedPart);

	Shape sphereShape = loadObj((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL)));
	sphereShape.normals = std::shared_ptr<Vec3>(new Vec3[sphereShape.vertexCount], std::default_delete<Vec3[]>());
	sphereShape.computeNormals(sphereShape.normals.get());

	PartFactory cubeFactory(BoundingBox{-0.49, -0.49, -0.49, 0.49, 0.49, 0.49}.toShape(new Vec3[8]), screen, "Cube");
	PartFactory sphereFactory(sphereShape, screen, "Sphere");
	PartFactory triangleFactory(triangleShape, screen, "Triangle");
	for(double x = 0; x < 4; x+=1.01) {
		for(double y = 0; y < 4; y += 1.01) {
			for(double z = 0; z < 4; z += 1.01) {
				world.addObject(cubeFactory.produce(CFrame(Vec3(x + 5, y + 1, z)), 1.0, 0.2));
				world.addObject(sphereFactory.produce(CFrame(Vec3(x - 5, y + 1, z)), 1.0, 0.2));
				world.addObject(triangleFactory.produce(CFrame(Vec3(x, y + 1, z)), 1.0, 0.2));
			}
		}
	}

	Shape stallShape = loadObj((std::istream&) std::istringstream(getResourceAsString(STALL_MODEL)));
	//stallShape.normals = std::shared_ptr<Vec3>(new Vec3[stallShape.vertexCount]);
	//stallShape.computeNormals(stallShape.normals.get());
	ExtendedPart* stallExtendedPart = createUniquePart(screen, stallShape, CFrame(Vec3(10.0, 2.0, -10.0), fromEulerAngles(0.1, 0.1, 0.1)), 10, 0.7);
	stallExtendedPart->material = Material(load("../res/textures/stall/stall.png"));
	world.addObject(stallExtendedPart);
	
	ExtendedPart* icosaExtendedPart = createUniquePart(screen, icosahedron, CFrame(Vec3(7.0, 2.0, -7.0), fromEulerAngles(0.1, 0.1, 0.1)), 10, 0.7);
	world.addObject(icosaExtendedPart);

	ExtendedPart* houseExtendedPart = createUniquePart(screen, house, CFrame(Vec3(-9.5, 1.0, -5.0), fromEulerAngles(0.7, 0.9, 0.7)), 1.0, 0.0);
	world.addObject(houseExtendedPart);

	player = sphereFactory.produce(CFrame(), 1.0, 0.2);
	player->properties.friction = 0;
	player->drawMeshId = -1;


	/* Loop until the user closes the window */
	Log::info("Started rendering");
	while (!screen.shouldClose()) {
		graphicsMeasure.mark(GraphicsProcess::UPDATE);
		screen.update();
		screen.refresh();
		graphicsMeasure.end();
	}

	Log::info("Closing screen");
	screen.close();
	Log::info("Closed screen");

	stop(0);
}

void init() {
	AppDebug::setupDebugHooks();

	if (!initGLFW()) {
		Log::error("GLFW not initialised");
		std::cin.get();
		stop(-1);
	}

	screen = Screen(800, 640, &world);

	if (!initGLEW()) {
		Log::error("GLEW not initialised");
		std::cin.get();
		stop(-1);
	}

	Log::info("Initializing screen");
	screen.init();
	Log::info("Initialized screen");

	setupPhysics();
}

void stop(int returnCode) {
	physicsThread.stop();

	glfwTerminate();
	exit(returnCode);
}

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

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, [](){
		physicsMeasure.mark(PhysicsProcess::OTHER);
		AppDebug::logTickStart();
		//try {
			world.tick(1 / physicsThread.getTPS());
		/*} catch(EngineException& ex) {
			Log::error("EngineException: \n %s", ex.what());
			__debugbreak();
		} catch(std::exception& ex) {
			Log::error("std::exception occurred! \n %s", ex.what());
			__debugbreak();
		} catch(const char* ex) {
			Log::error("char* exception occurred! \n %s", ex);
			__debugbreak();
		}*/
		AppDebug::logTickEnd();
		physicsMeasure.end();
	});
}

void toggleFlying() {
	if(flying) {
		player->cframe = screen.camera.cframe;
		screen.camera.attachment = player;
		screen.world->addObject(player);
		flying = false;
	} else {
		screen.camera.attachment = nullptr;
		screen.world->removePart(player);
		flying = true;
	}
}

Camera& getCamera() {
	return screen.camera;
}
