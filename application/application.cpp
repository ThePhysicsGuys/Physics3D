#include "core.h"

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

#include "debug.h"
#include "worlds.h"
#include "tickerThread.h"
#include "resourceLoader.h"
#include "worldBuilder.h"

#include "io/export.h"
#include "io/import.h"

#include "resource/resourceManager.h"
#include "resource/textureResource.h"

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
		screen.onUpdate();
		screen.onRender();
		graphicsMeasure.end();
	}

	stop(0);
}

void init() {
	setupScreen();
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

double getYOffset(double x, double z) {
	return -10 * cos(x / 30.0 + z / 20.0) - 7 * sin(-x / 25.0 + z / 17.0 + 2.4) + 2 * sin(x / 4.0 + z / 7.0) + sin(x / 9.0 - z / 3.0) + sin(-x / 3.0 + z / 5.0);
}

void buildTerrain() {
	PartFactory treeTrunkfactory(Library::createPrism(7, 0.5, 11.0), screen, "TreeTrunk");
	PartFactory icosahedronFactory(Library::icosahedron.scaled(4.0, 4.0, 4.0), screen, "Icosahedron");
	PartFactory leafFactory(Library::icosahedron.scaled(2.0, 2.3, 1.9), screen, "Leaf");
	for (int x = -50; x < 50; x++) {
		for (int z = -50; z < 50; z++) {
			double yOffset = getYOffset(x * 3.0, z * 3.0);
			Position pos((x + fRand(0.0, 1.0)) * 3.0, fRand(0.0, 1.0) + yOffset, (z + fRand(0.0, 1.0)) * 3.0);
			GlobalCFrame cf(pos, fromEulerAngles(fRand(0.0, 3.1415), fRand(0.0, 3.1415), fRand(0.0, 3.1415)));
			ExtendedPart* newPart = icosahedronFactory.produce(cf, 1.0, 1.0, "terrain");
			newPart->material.ambient = Vec4(0.0, yOffset / 40.0 + 0.5, 0.0, 1.0);
			world.addTerrainPart(newPart);
		}
	}

	for (int i = 0; i < 200; i++) {
		double x = fRand(-150, 150);
		double z = fRand(-150, 150);


		Position treePos(x, getYOffset(x, z) + 8.0, z);

		GlobalCFrame trunkCFrame(treePos, fromEulerAngles(fRand(-0.1, 0.1), fRand(-3.1415, 3.1415), fRand(-0.1, 0.1)));

		ExtendedPart* trunk = treeTrunkfactory.produce(trunkCFrame, 1.0, 1.0, "trunk");
		trunk->material.ambient = GUI::COLOR::get(0x654321);
		world.addTerrainPart(trunk);

		Position treeTop = trunkCFrame.localToGlobal(Vec3(0.0, 5.5, 0.0));


		for (int j = 0; j < 15; j++) {

			GlobalCFrame leavesCFrame(treeTop + Vec3(fRand(-1.0, 1.0), fRand(-1.0, 1.0), fRand(-1.0, 1.0)), fromEulerAngles(fRand(0.0, 3.1415), fRand(0.0, 3.1415), fRand(0.0, 3.1415)));
			ExtendedPart* leaves = leafFactory.produce(leavesCFrame, 1.0, 1.0, "trunk");

			leaves->material.ambient = Vec4(fRand(-0.2, 0.2), 0.6 + fRand(-0.2, 0.2), 0.0, 1.0);

			world.addTerrainPart(leaves);
		}
		
	}
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
	PartFactory cubeFactory(Library::createCube(1.0), screen, "Cube");
	PartFactory bigCubeFactory(Library::createCube(2.0), screen, "Cube");
	PartFactory sphereFactory(sphereShape, screen, "Sphere");
	PartFactory smallSphereFactory(sphereShape.scaled(0.5, 0.5, 0.5), screen, "SmallSphere");
	PartFactory triangleFactory(Library::trianglePyramid, screen, "Triangle");
	PartFactory wedgeFactory(Library::wedge, screen, "Wedge");

	// Floor
	Vec2 floorSize(50.0, 50.0);
	double wallHeight = 7.0;
	ResourceManager::add<TextureResource>("../res/textures/floor/floor_color.jpg", "floorMaterial");
	Material floorMaterial = Material(ResourceManager::get<TextureResource>("floorMaterial"));
	ExtendedPart* floorExtendedPart = createUniquePart(screen, BoundingBox(floorSize.x, 1.0, floorSize.y).toShape(), GlobalCFrame(0.0, 0.0, 0.0), 2.0, 1.0);
	floorExtendedPart->material = floorMaterial;
	world.addTerrainPart(floorExtendedPart);

	// Walls
	PartFactory xWallFactory(BoundingBox(0.7, wallHeight, floorSize.y - 0.7).toShape(), screen, "xWall");
	PartFactory zWallFactory(BoundingBox(floorSize.x, wallHeight, 0.7).toShape(), screen, "zWall");
	world.addTerrainPart(xWallFactory.produce(GlobalCFrame(Position(floorSize.x / 2, wallHeight / 2, 0.0)), 2.0, 1.0));
	world.addTerrainPart(zWallFactory.produce(GlobalCFrame(Position(0.0, wallHeight / 2, floorSize.y / 2)), 2.0, 1.0));
	world.addTerrainPart(xWallFactory.produce(GlobalCFrame(Position(-floorSize.x / 2, wallHeight / 2, 0.0)), 2.0, 1.0));
	world.addTerrainPart(zWallFactory.produce(GlobalCFrame(Position(0.0, wallHeight / 2, -floorSize.y / 2)), 2.0, 1.0));

	// hollow box
	/*WorldBuilder::HollowBoxParts parts = WorldBuilder::buildHollowBox(Bounds(Position(12.0, 3.0, 14.0), Position(20.0, 8.0, 20.0)), 0.3);

	parts.front->material.ambient = Vec4f(0.4, 0.6, 1.0, 0.3);
	parts.back->material.ambient = Vec4f(0.4, 0.6, 1.0, 0.3);*/

	// Rotating walls
	/*PartFactory rotatingWallFactory(BoundingBox(5.0, 3.0, 0.5).toShape(), screen, "rotatingWall");
	ExtendedPart* rotatingWall = rotatingWallFactory.produce(GlobalCFrame(Position(-12.0, 1.7, 0.0)), 20000000.0, 1.0);
	ExtendedPart* rotatingWall2 = rotatingWallFactory.produce(GlobalCFrame(Position(-12.0, 1.7, 5.0)), 20000000.0, 1.0);
	world.addPart(rotatingWall, true);
	world.addPart(rotatingWall2, true);
	rotatingWall->parent->angularVelocity = Vec3(0, -0.7, 0);
	rotatingWall2->parent->angularVelocity = Vec3(0, 0.7, 0);*/

	// Many many parts
	/*for (int i = 0; i < 3; i++) {
		ExtendedPart* newCube = cubeFactory.produce(GlobalCFrame(fRand(-10.0, 0.0), fRand(1.0, 1.0), fRand(-10.0, 0.0)), 1.0, 0.2);
		world.addPart(newCube);
	}*/

	ExtendedPart* carBody = cubeFactory.produceScaled(GlobalCFrame(5.0, 1.0, 5.0), 1.0, 0.7, 2.0, 0.1, 1.0, "CarBody");
	ExtendedPart* carLeftPanel =	cubeFactory.produceScaled(carBody, CFrame(0.0, 0.25, -0.5), 1.0, 0.7, 2.0, 0.4, 0.1, "CarLeftSide");
	ExtendedPart* carRightPanel =	cubeFactory.produceScaled(carBody, CFrame(0.0, 0.25, 0.5), 1.0, 0.7, 2.0, 0.4, 0.1, "CarRightSide");
	//ExtendedPart* carLeftWindow =	cubeFactory.produceScaled(carBody, CFrame(-0.3, 0.85, -0.5), 1.0, 0.7, 1.4, 0.8, 0.1, "WindowLeft");
	ExtendedPart* carLeftWindow =	cubeFactory.produceScaled(1.0, 0.7, 1.4, 0.8, 0.05, "WindowLeft");
	ExtendedPart* carWedgeLeft =	wedgeFactory.produceScaled(carLeftWindow, CFrame(1.0, 0.0, 0.0), 1.0, 0.7, 0.6, 0.8, 0.1, "WedgeLeft");
	carLeftPanel->attach(*carLeftWindow, CFrame(-0.3, 0.6, 0.0));
	ExtendedPart* carRightWindow =	cubeFactory.produceScaled(1.0, 0.7, 1.4, 0.8, 0.05, "WindowRight");
	ExtendedPart* carWedgeRight =	wedgeFactory.produceScaled(carRightWindow, CFrame(1.0, 0.0, 0.0), 1.0, 0.7, 0.6, 0.8, 0.1, "WedgeRight");
	carRightPanel->attach(*carRightWindow, CFrame(-0.3, 0.6, 0.0));
	ExtendedPart* carFrontPanel =	cubeFactory.produceScaled(carBody, CFrame(1.0, 0.25, 0.0), 1.0, 0.7, 0.1, 0.4, 1.0, "FrontPanel");
	ExtendedPart* carTrunkPanel =	cubeFactory.produceScaled(carBody, CFrame(-1.0, 0.65, 0.0), 1.0, 0.7, 0.1, 1.2, 1.0, "TrunkPanel");
	ExtendedPart* carRoof =			cubeFactory.produceScaled(carBody, CFrame(-0.3, 1.25, 0.0), 1.0, 0.7, 1.4, 0.1, 1.0, "Roof");

	ExtendedPart* carWindshield =	cubeFactory.produceScaled(carBody, CFrame(Vec3(0.7, 0.85, 0.0), fromEulerAngles(0.0, 0.0, -0.91)), 1.0, 0.7, 1.0, 0.05, 1.0, "Windshield");
	ExtendedPart* wheel1 = smallSphereFactory.produce(GlobalCFrame(5.8, 1.0, 5.8), 1.0, 1.0, "Wheel");
	ExtendedPart* wheel2 = smallSphereFactory.produce(GlobalCFrame(5.8, 1.0, 4.2), 1.0, 1.0, "Wheel");
	ExtendedPart* wheel3 = smallSphereFactory.produce(GlobalCFrame(4.2, 1.0, 5.8), 1.0, 1.0, "Wheel");
	ExtendedPart* wheel4 = smallSphereFactory.produce(GlobalCFrame(4.2, 1.0, 4.2), 1.0, 1.0, "Wheel");

	carLeftWindow->material.ambient = Vec4f(0.7, 0.7, 1.0, 0.5);
	carRightWindow->material.ambient = Vec4f(0.7, 0.7, 1.0, 0.5);
	carWindshield->material.ambient = Vec4f(0.7, 0.7, 1.0, 0.5);

	world.addPart(carBody);

	world.addPart(wheel1);
	world.addPart(wheel2);
	world.addPart(wheel3);
	world.addPart(wheel4);

	ConstraintGroup car;
	car.ballConstraints.push_back(BallConstraint{ Vec3(0.8, 0.0, 0.8), carBody->parent, Vec3(0,0,0), wheel1->parent });
	car.ballConstraints.push_back(BallConstraint{ Vec3(0.8, 0.0, -0.8), carBody->parent, Vec3(0,0,0), wheel2->parent });
	car.ballConstraints.push_back(BallConstraint{ Vec3(-0.8, 0.0, 0.8), carBody->parent, Vec3(0,0,0), wheel3->parent });
	car.ballConstraints.push_back(BallConstraint{ Vec3(-0.8, 0.0, -0.8), carBody->parent, Vec3(0,0,0), wheel4->parent });
	world.constraints.push_back(std::move(car));

	int minX = -2;
	int maxX = 2;
	int minY = 0;
	int maxY = 5;
	int minZ = -2;
	int maxZ = 2;

	for (double x = minX; x < maxX; x += 1.01) {
		for (double y = minY; y < maxY; y += 1.01) {
			for (double z = minZ; z < maxZ; z += 1.01) {
				ExtendedPart* newCube = cubeFactory.produce(GlobalCFrame(x - 5, y + 1, z - 5), 1.0, 0.2);
				newCube->material.ambient = Vec4f((x-minX)/(maxX-minX), (y-minY)/(maxY-minY), (z-minZ)/(maxZ-minZ), 1.0f);
				world.addPart(newCube);
				world.addPart(sphereFactory.produce(GlobalCFrame(Position(x + 5, y + 1, z - 5)), 1.0, 0.2));
				spiderFactories[rand() & 0x00000003].buildSpider(GlobalCFrame(Position(x+y*0.1, y+1, z)));
				world.addPart(triangleFactory.produce(GlobalCFrame(Position(x - 20, y + 1, z + 20)), 1.0, 0.2));
			}
		}
	}
	//buildTerrain();
	//world.optimizeTerrain();

	// Player
	screen.camera.attachment = sphereFactory.produce(GlobalCFrame(), 1.0, 0.2);
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
	screen.onClose();

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
	if (screen.camera.flying) {
		screen.camera.flying = false;
		screen.camera.attachment->setCFrame(screen.camera.cframe);
		screen.world->addPart(screen.camera.attachment);
	} else {
		screen.world->removePart(screen.camera.attachment);
		screen.camera.flying = true;
	}
}
