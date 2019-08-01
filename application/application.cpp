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
#include "view/texture.h"
#include "view/debug/visualDebug.h"
#include "view/material.h"

#include "../engine/math/mathUtil.h"

#include "../engine/geometry/shape.h"
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

#include "worldBuilder.h"

#include "io/export.h"
#include "io/import.h"

#define _USE_MATH_DEFINES
#include "math.h"

#define TICKS_PER_SECOND 120.0

#define TICK_SKIP_TIME std::chrono::milliseconds(3000)

Screen screen;
GravityWorld world(Vec3(0.0, -10.0, 0.0));

TickerThread physicsThread;

ExtendedPart* player;
bool flying = true;

void init();
void setupPhysics();

double fRand(double fMin, double fMax) {
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

int main(void) {
	init();

	VisualShape sphereShape = OBJImport::load((std::istream&) std::istringstream(getResourceAsString(SPHERE_MODEL)));
	Vec3f* normalBuf = new Vec3f[sphereShape.vertexCount];
	sphereShape.computeNormals(normalBuf);
	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuf);

	dominoFactory = PartFactory(dominoShape, screen, "domino");
	legFactory = PartFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");

	SpiderFactory spiderFactories[]{ {0.5, 3},{0.5, 4},{0.5, 5},{0.5, 6} };
	PartFactory cubeFactory(createCube(0.2), screen, "Cube");
	PartFactory sphereFactory(sphereShape, screen, "Sphere");
	PartFactory triangleFactory(triangleShape, screen, "Triangle");


	int* builderRemovalBuffer = new int[1000];
	EdgePiece* builderAddingBuffer = new EdgePiece[1000];


	//world.addPart(dominoFactory.produce(CFrame(Vec3(1.5, 0.7, -7.3), fromEulerAngles(0.0, 0.2, 0.0)), 2.0, 0.7));

	Vec2 floorSize(30.0, 30.0);
	double wallHeight = 7.0;

	Material floorMaterial = Material(load("../res/textures/floor/floor_color.jpg"));

	ExtendedPart* floorExtendedPart = createUniquePart(screen, BoundingBox(floorSize.x, 1.0, floorSize.y).toShape(), CFrame(Vec3(0.0, -0.15, 0.0)), 2.0, 1.0);
	floorExtendedPart->material = floorMaterial;
	world.addPart(floorExtendedPart, true);

	PartFactory xWallFactory(BoundingBox(0.7, wallHeight, floorSize.y).toShape(), screen, "xWall");
	PartFactory zWallFactory(BoundingBox(floorSize.x, wallHeight, 0.7).toShape(), screen, "zWall");

	/*world.addPart(xWallFactory.produce(CFrame(Vec3(floorSize.x / 2, wallHeight / 2, 0.0)), 0.2, 1.0), true);
	world.addPart(zWallFactory.produce(CFrame(Vec3(0.0, wallHeight / 2, floorSize.y / 2)), 0.2, 1.0), true);
	world.addPart(xWallFactory.produce(CFrame(Vec3(-floorSize.x / 2, wallHeight / 2, 0.0)), 0.2, 1.0), true);
	world.addPart(zWallFactory.produce(CFrame(Vec3(0.0, wallHeight / 2, -floorSize.y / 2)), 0.2, 1.0), true);*/

	ExtendedPart* ramp = createUniquePart(screen, BoundingBox(10.0, 0.17, 3.0).toShape(), CFrame(Vec3(12.0, 1.5, 0.0), fromEulerAngles(M_PI / 2 * 0.2, M_PI/2, 0.0)), 0.2, 1.0);
	//world.addPart(ramp, true);

	PartFactory rotatingWallFactory(BoundingBox(5.0, 3.0, 0.5).toShape(), screen, "rotatingWall");
	ExtendedPart* rotatingWall = rotatingWallFactory.produce(CFrame(Vec3(-12, 1.5, 0.0)), 0.2, 1.0);
	//world.add(rotatingWall, true);
	//rotatingWall->parent->angularVelocity = Vec3(0, -0.7, 0);

	ExtendedPart* rotatingWall2 = rotatingWallFactory.produce(CFrame(Vec3(-12, 1.5, 5.0)), 0.2, 1.0);
	//world.add(rotatingWall2, true);
	//rotatingWall2->parent->angularVelocity = Vec3(0, 0.7, 0);

	// makeDominoStrip(20);
	// makeDominoTower(20, 10, Vec3(-4.0, 0.0, -4.0));

	Vec3f newIcosaVerts[30];
	Triangle newIcosaTriangles[40];
	TriangleNeighbors icosaNeighBuf[40];

	ConvexShapeBuilder icosaBuilder(icosahedron, newIcosaVerts, newIcosaTriangles, icosaNeighBuf, builderRemovalBuffer, builderAddingBuffer);

	if(!icosaBuilder.toIndexedShape().isValid()) {
		throw "BAD";
	}

	/*HollowBoxParts hollowBox = buildHollowBox(BoundingBox(3.0, 3.0, 4.0, 7.0, 6.0, 7.0), 0.2);
	
	hollowBox.front->material.ambient = Vec4f(0.5, 0.7, 1.0, 0.5);
	hollowBox.back->material.ambient = Vec4f(0.5, 0.7, 1.0, 0.5);
	hollowBox.bottom->material.ambient = Vec4f(0.9, 0.9, 0.9, 1.0);
	hollowBox.top->material.ambient = Vec4f(0.9, 0.9, 0.9, 1.0);*/

	icosaBuilder.addPoint(Vec3f(0, 1.1, 0));
	icosaBuilder.addPoint(Vec3f(0, -1.1, 0));
	icosaBuilder.addPoint(Vec3f(1.1, 0, 0));
	icosaBuilder.addPoint(Vec3f(-1.1, 0, 0));
	icosaBuilder.addPoint(Vec3f(0, 0, 1.1));
	icosaBuilder.addPoint(Vec3f(0, 0, -1.1));

	if(!icosaBuilder.toIndexedShape().isValid()) {
		throw "BAD";
	}

	Shape newIcosa = icosaBuilder.toShape();
	ExtendedPart* constructedIcosa = createUniquePart(screen, newIcosa, CFrame(Vec3(10, 0, -10)), 2.0, 0.7);

	Vec3f verts[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	ConvexShapeBuilder builder(verts, triangles, 4, 4, neighBuf, builderRemovalBuffer, builderAddingBuffer);

	builder.addPoint(Vec3(0.4, 0.4, 0.4), 3);

	builder.addPoint(Vec3(-0.4, 1, -0.4));

	builder.addPoint(Vec3(-0.8, 0.5, -0.8));

	builder.addPoint(Vec3(-0.9, 0.6, -0.9));

	Shape constructedShape = builder.toShape();

	ExtendedPart* constructedExtendedPart = createUniquePart(screen, constructedShape, CFrame(Vec3(-10.0, 2.0, -10.0)), 2.0, 0.7);
	//world.addPart(constructedExtendedPart);


	/*for (int i = 0; i < 10000; i++) {
		ExtendedPart* newCube = cubeFactory.produce(CFrame(Vec3(fRand(-10.0, 10.0), fRand(0.0, 20.0), fRand(-10.0, 10.0))), 1.0, 0.2);
		world.addPart(newCube);
	}*/

	//Physical* alonePhysical = new Physical(cubeFactory.produce(CFrame(Vec3(0, 0, 0)), 1.0, 1.0, "P1"));

	/*ExtendedPart* alonePart = cubeFactory.produce(CFrame(Vec3(0, 0, 0)), 1.0, 1.0, "P1");

	world.addPart(alonePart);

	alonePart->parent->velocity = Vec3(10, 0, 0);

	world.addPart(cubeFactory.produce(CFrame(Vec3(10, 0, 0)), 1.0, 1.0, "P1"));
	world.addPart(cubeFactory.produce(CFrame(Vec3(10, 0, 1)), 1.0, 1.0, "P1"));
	world.addPart(cubeFactory.produce(CFrame(Vec3(10, 0, -1)), 1.0, 1.0, "P1"));
	world.addPart(cubeFactory.produce(CFrame(Vec3(10, 1, 0)), 1.0, 1.0, "P1"));
	world.addPart(cubeFactory.produce(CFrame(Vec3(10, 1, 1)), 1.0, 1.0, "P1"));
	world.addPart(cubeFactory.produce(CFrame(Vec3(10, 1, -1)), 1.0, 1.0, "P1"));*/

	// Tree test
	/*Physical* a = new Physical(cubeFactory.produce(CFrame(Vec3(0, 0, -1)), 1.0, 1.0, "A"));
	Physical* b = new Physical(cubeFactory.produce(CFrame(Vec3(0, 0, 1)), 1.0, 1.0, "B"));
	Physical* c = new Physical(cubeFactory.produce(CFrame(Vec3(0.5, 0, -1)), 1.0, 1.0, "C"));
	Physical* d = new Physical(cubeFactory.produce(CFrame(Vec3(0.5, 0, 1)), 1.0, 1.0, "D"));

	TreeNode nodeAB(new TreeNode[4]{TreeNode(a, a->getStrictBounds()), TreeNode(b, b->getStrictBounds()) }, 2);
	TreeNode nodeCD(new TreeNode[4]{ TreeNode(c, c->getStrictBounds()), TreeNode(d, d->getStrictBounds()) }, 2);

	world.objectTree.rootNode.add(std::move(nodeAB));
	world.objectTree.rootNode.add(std::move(nodeCD));

	world.physicals.add(a);
	world.physicals.add(b);
	world.physicals.add(c);
	world.physicals.add(d);

	world.objectTree.rootNode.recalculateBoundsRecursive(true);*/

	//world.objectTree.add(TreeNode())

	int minX = -5;
	int maxX = 5;
	int minY = 0;
	int maxY = 5;
	int minZ = -5;
	int maxZ = 5;

	for(double x = minX; x < maxX; x+=1.01) {
		for(double y = minY; y < maxY; y += 1.01) {
			for(double z = minZ; z < maxZ; z += 1.01) {
				ExtendedPart* newCube = cubeFactory.produce(CFrame(Vec3(x, y + 1, z)), 1.0, 0.2);
				newCube->material.ambient = Vec4f((x-minX)/(maxX-minX), (y-minY)/(maxY-minY), (z-minZ)/(maxZ-minZ), 1.0f);
				//world.addPart(newCube);
				//world.addPart(sphereFactory.produce(CFrame(Vec3(x, y + 1, z - 5)), 1.0, 0.2));
				world.addPart(triangleFactory.produce(CFrame(Vec3(x, y + 1, z + 5)), 1.0, 0.2));


				//spiderFactories[rand() & 0x00000003].buildSpider(CFrame(Vec3(x+y*0.1, y+1, z)));
			}
		}
	}



	//Shape stallShape = OBJImport::load((std::istream&) std::istringstream(getResourceAsString(STALL_MODEL)));
	//Shape stallShape = OBJImport::load("../res/models/gui/stall.obj");

	//ExtendedPart* stallExtendedPart = createUniquePart(screen, stallShape, CFrame(Vec3(0.0, 10.0, 0.0), fromEulerAngles(0.1, 0.1, 0.1)), 10, 0.7);
	//stallExtendedPart->material = Material(load("../res/textures/stall/stall.png"));
	//world.addPart(stallExtendedPart);
	
	ExtendedPart* icosaExtendedPart = createUniquePart(screen, icosahedron, CFrame(Vec3(7.0, 2.0, -7.0), fromEulerAngles(0.1, 0.1, 0.1)), 10, 0.7);
	//world.addPart(icosaExtendedPart);

	ExtendedPart* houseExtendedPart = createUniquePart(screen, house, CFrame(Vec3(-9.5, 1.0, -5.0), fromEulerAngles(0.7, 0.9, 0.7)), 1.0, 0.0);
	//world.addPart(houseExtendedPart);
	
	Shape box(BoundingBox(1.0, 0.5, 0.5).toShape());
	Shape box2(BoundingBox(0.5, 0.5, 1.0).toShape());
	Shape doubleBox(BoundingBox(2.0, 0.5, 0.5).toShape());
	ExtendedPart* p1 = createUniquePart(screen, box, CFrame(Vec3(0,2,0), fromEulerAngles(0.7, 0.6, 0.3)), 10.0, 0.0, "p1");
	ExtendedPart* p2 = createUniquePart(screen, box2, CFrame(Vec3(0, 3, 0)), 10.0, 0.0, "p2");
	ExtendedPart* doubleP = createUniquePart(screen, doubleBox, CFrame(), 10.0, 0, "doubleP");

	p1->material.ambient = Vec4f(1, 0, 0, 1);
	p2->material.ambient = Vec4f(0, 1, 0, 1);

	//world.addPart(p1);
	//world.addPart(p2);
	//p1->parent->attachPart(p2, CFrame(Vec3(1.0, 0.0, 0.0), Mat3(0, 0, 1, 0, 1, 0, -1, 0, 0)));

	//Physical phys2(doubleP);

	//world.addPart(doubleP);

	//world.attachPart(p2, *(p1->parent), CFrame(Vec3(1.0, 0.0, 0.0), Mat3(0, 0, 1, 0, 1, 0, -1, 0, 0)));

	//p1->parent->angularVelocity = Vec3(0, 4, 4);

	

	//p1->parent->velocity = Vec3(0,1,0);

	//Physical phys2(doubleP);



	/*ASSERT(phys.mass == p1->mass + p2->mass);
	ASSERT(phys.centerOfMass == Vec3(0.5, 0, 0));
	ASSERT(phys.inertia == phys2.inertia);*/

	TreeIteratorIntersectingBounds iter(world.objectTree.rootNode, Bounds(Position(10.0, 10.0, 10.0), Position(11.0, 11.0, 11.0)));
	for (; iter != TreeIteratorEnd(); ++iter) {
		TreeNode* p = *iter;

		static_cast<ExtendedPart*>(static_cast<Physical*>(p->object)->mainPart)->material.ambient = Vec4f(1.0, 1.0, 0.0, 1.0);
	}

	if (!world.isValid()) {
		throw "World not valid!";
	}
	
	player = sphereFactory.produce(CFrame(), 1.0, 0.2);
	player->properties.friction = 0.5;
	player->drawMeshId = -1;

	//saveWorld("testWorld", world);
	//loadWorld("testWorld", world, getCamera());

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
		GJKCollidesIterationStatistics.nextTally();
		GJKNoCollidesIterationStatistics.nextTally();
		EPAIterationStatistics.nextTally();
	});
}

void toggleFlying() {
	if(flying) {
		player->cframe = screen.camera.cframe;
		screen.camera.attachment = player;
		screen.world->addPart(player);
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
