#include "core.h"

#include "application.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "view/screen.h"
#include "ecs/material.h"

#include "../graphics/resources.h"
#include "../graphics/texture.h"
#include "../graphics/debug/debug.h"
#include "../graphics/debug/visualDebug.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/basicShapes.h"
#include "../physics/math/mathUtil.h"
#include "../physics/math/linalg/commonMatrices.h"
#include "../physics/part.h"
#include "../physics/world.h"
#include "../physics/misc/gravityForce.h"
#include "../physics/physicsProfiler.h"
#include "../physics/constraints/motorConstraint.h"
#include "../physics/constraints/fixedConstraint.h"

#include "../physics/misc/serialization.h"

#include "worlds.h"
#include "tickerThread.h"
#include "worldBuilder.h"

#include "io/serialization.h"

#include "../util/resource/resourceLoader.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/resource/textureResource.h"
#include "../engine/io/import.h"

#define TICKS_PER_SECOND 120.0
#define TICK_SKIP_TIME std::chrono::milliseconds(1000)

namespace Application {

TickerThread physicsThread;
PlayerWorld world(1 / TICKS_PER_SECOND);
Screen screen;

void init(int argc, const char** args);
void setupPhysics();
void registerShapes();
void setupWorld(int argc, const char** args);
void setupGL();
void setupDebug();

bool has_suffix(const std::string& str, const std::string& suffix) {
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void init(int argc, const char** args) {
	setupGL();
	registerShapes();
	WorldBuilder::init();

	if(argc >= 2) {
		const char* file = args[1];
		Log::info("Loading file %s", file);
		auto startTime = std::chrono::high_resolution_clock::now();
		if(has_suffix(file, ".parts")) {
			WorldImportExport::loadLoosePartsIntoWorld(file, world);
		} else if(has_suffix(file, ".nativeParts")) {
			WorldImportExport::loadNativePartsIntoWorld(file, world);
		} else if(has_suffix(file, ".world")) {
			world.addExternalForce(new ExternalGravity(Vec3(0, -10.0, 0.0)));
			WorldImportExport::loadWorld(file, world);
		}
		std::chrono::nanoseconds deltaTime = std::chrono::high_resolution_clock::now() - startTime;
		Log::info("File loaded, took %.4f ms", deltaTime.count() / 1E6);
	} else {
		setupWorld(argc, args);
	}
	
	
	Log::info("Initializing screen");
	screen.onInit();
	


	// Player
	screen.camera.attachment = new ExtendedPart(Library::createPrism(50, 0.3, 1.5), GlobalCFrame(), {1.0, 5.0, 0.0}, "Player");

	if(!world.isValid()) {
		throw "World not valid!";
	}

	setupPhysics();
	setupDebug();
}

void setupGL() {
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
}

// Generates a cylinder with 
static VisualShape createCylinder(int sides, double radius, double height) {
	int vertexCount = sides * 4;
	Vec3f* vecBuf = new Vec3f[vertexCount];


	// vertices
	for (int i = 0; i < sides; i++) {
		double angle = i * PI * 2 / sides;
		Vec3f bottom(cos(angle) * radius, sin(angle) * radius, height / 2);
		Vec3f top(cos(angle) * radius, sin(angle) * radius, -height / 2);
		vecBuf[i * 2] = bottom;
		vecBuf[i * 2 + 1] = top;
		vecBuf[i * 2 + sides * 2] = bottom;
		vecBuf[i * 2 + 1 + sides * 2] = top;
	}

	int triangleCount = sides * 2 + (sides - 2) * 2;
	Triangle* triangleBuf = new Triangle[triangleCount];

	// sides
	for (int i = 0; i < sides; i++) {
		int botLeft = i * 2;
		int botRight = ((i + 1) % sides) * 2;
		triangleBuf[i * 2] = Triangle { botLeft, botLeft + 1, botRight }; // botLeft, botRight, topLeft
		triangleBuf[i * 2 + 1] = Triangle { botRight + 1, botRight, botLeft + 1 }; // topRight, topLeft, botRight
	}

	Vec3f* normals = new Vec3f[vertexCount];

	for (int i = 0; i < sides * 2; i++) {
		Vec3f vertex = vecBuf[i];
		normals[i] = normalize(Vec3(vertex.x, vertex.y, 0));
	}

	Triangle* capOffset = triangleBuf + sides * 2;
	// top and bottom
	for (int i = 0; i < sides - 2; i++) { // common corner is i=0
		capOffset[i] = Triangle { sides * 2 + 0, sides * 2 + (i + 1) * 2, sides * 2 + (i + 2) * 2 };
		capOffset[i + (sides - 2)] = Triangle { sides * 2 + 1, sides * 2 + (i + 2) * 2 + 1, sides * 2 + (i + 1) * 2 + 1 };
	}

	for (int i = 0; i < sides; i++) {
		normals[i * 2 + sides * 2] = Vec3f(0, 0, 1);
		normals[i * 2 + sides * 2 + 1] = Vec3f(0, 0, -1);
	}

	return VisualShape(vecBuf, SharedArrayPtr<const Vec3f>(normals), triangleBuf, vertexCount, triangleCount);
}

static void registerShapes() {
	Polyhedron sphere(Library::createSphere(1.0, 3));
	VisualShape sphereShape = VisualShape(sphere);
	Vec3f* normalBuf = new Vec3f[sphereShape.vertexCount];
	sphereShape.computeNormals(normalBuf);
	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuf);

	screen.registerMeshFor(Sphere(1.0).baseShape, sphereShape);

	screen.registerMeshFor(Box(2.0, 2.0, 2.0).baseShape);

	screen.registerMeshFor(Cylinder(1.0, 2.0).baseShape, createCylinder(64, 1.0, 2.0));
}



void setupWorld(int argc, const char** args) {
	Log::info("Initializing world");

	world.addExternalForce(new ExternalGravity(Vec3(0, -10.0, 0.0)));

	// WorldBuilder init

	// Part factories
	//WorldBuilder::SpiderFactory spiderFactories[] { {0.5, 4},{0.5, 6},{0.5, 8},{0.5, 10} };
	//Shape triangle(Library::trianglePyramid);

	WorldBuilder::buildFloorAndWalls(50.0, 50.0, 1.0);

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
	/*ExtendedPart* rotatingWall = new ExtendedPart(Box(5.0, 3.0, 0.5), GlobalCFrame(Position(-12.0, 1.7, 0.0)), {1.0, 1.0, 0.7});
	ExtendedPart* rotatingWall2 = new ExtendedPart(Box(5.0, 3.0, 0.5), GlobalCFrame(Position(-12.0, 1.7, 5.0)), {1.0, 1.0, 0.7});
	world.addPart(rotatingWall, true);
	world.addPart(rotatingWall2, true);
	rotatingWall->parent->mainPhysical->motionOfCenterOfMass.angularVelocity = Vec3(0, -0.7, 0);
	rotatingWall2->parent->mainPhysical->motionOfCenterOfMass.angularVelocity = Vec3(0, 0.7, 0);*/

	// Many many parts
	for (int i = 0; i < 3; i++) {
		ExtendedPart* newCube = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(fRand(-10.0, 0.0), fRand(1.0, 1.0), fRand(-10.0, 0.0)), {1.0, 0.2, 0.7});
		world.addPart(newCube);
	}

	WorldBuilder::buildCar(GlobalCFrame(5.0, 1.0, 5.0));


	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-10.0, 1.0, -10.0, fromEulerAngles(0.15, 0.0, 0.0)), 1.5);
	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-12.5, 1.0, -14.0, fromEulerAngles(0.0, 3.1415/2, -0.15)), 1.5);
	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-16.5, 1.0, -11.5, fromEulerAngles(-0.15, 0.0, -0.0)), -1.5);
	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-14.0, 1.0, -7.5, fromEulerAngles(0.0, 3.1415 / 2, 0.15)), -1.5);

	int minX = 0;
	int maxX = 3;
	int minY = 0;
	int maxY = 3;
	int minZ = 0;
	int maxZ = 3;

	GlobalCFrame rootFrame(Position(0.0, 15.0, 0.0), fromEulerAngles(3.1415 / 4, 3.1415 / 4, 0.0));

	for (double x = minX; x < maxX; x += 1.00001) {
		for (double y = minY; y < maxY; y += 1.00001) {
			for (double z = minZ; z < maxZ; z += 1.00001) {
				ExtendedPart* newCube = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(Position(x - 5, y + 10, z - 5)), { 1.0, 1.0, 0.0 }, "Box");
				newCube->material.ambient = Vec4f((x-minX)/(maxX-minX), (y-minY)/(maxY-minY), (z-minZ)/(maxZ-minZ), 1.0f);
				world.addPart(newCube);
				world.addPart(new ExtendedPart(Sphere(0.5), GlobalCFrame(Position(x + 5, y + 1, z - 5)), { 1.0, 0.2, 0.5 }, "Sphere"));
				//spiderFactories[rand() & 0x00000003].buildSpider(GlobalCFrame(Position(x+y*0.1, y+1, z)));
				//world.addPart(new ExtendedPart(triangle, GlobalCFrame(Position(x - 20, y + 1, z + 20)), { 1.0, 0.2, 0.5 }, "Triangle"));

				world.addPart(new ExtendedPart(Cylinder(0.3, 1.2), GlobalCFrame(x - 5, y + 1, z + 5, fromEulerAngles(3.1415/4, 3.1415/4, 0.0)), {1.0, 0.2, 0.5}, "Cylinder"));
			}
		}
	}

	WorldBuilder::buildTerrain(500.0, 500.0);


	ExtendedPart* ropeStart = new ExtendedPart(Box(2.0, 1.5, 0.7), GlobalCFrame(10.0, 2.0, -10.0), {1.0, 0.7, 0.3}, "RopeA");
	ExtendedPart* ropeB = new ExtendedPart(Box(1.5, 1.2, 0.9), GlobalCFrame(10.0, 2.0, -14.0), {1.0, 0.7, 0.3}, "RopeB");
	ExtendedPart* ropeC = new ExtendedPart(Box(2.0, 1.5, 0.7), GlobalCFrame(10.0, 2.0, -18.0), {1.0, 0.7, 0.3}, "RopeC");

	world.addPart(ropeStart);
	world.addPart(ropeB);
	world.addPart(ropeC);

	ConstraintGroup group;

	group.ballConstraints.push_back(BallConstraint{Vec3(0.0, 0.0, -2.0), ropeStart->parent, Vec3(0.0, 0.0, 2.0), ropeB->parent});
	group.ballConstraints.push_back(BallConstraint{Vec3(0.0, 0.0, -2.0), ropeB->parent, Vec3(0.0, 0.0, 2.0), ropeC->parent});

	world.constraints.push_back(group);//*/

	/*MotorConstraint mmmmmm(Vec3(0.5, -0.5, 0.6));

	std::ofstream file;
	file.open("motorConstraint.bin", std::ios::binary);
	hardConstraintRegistry.serialize(mmmmmm, file);
	file.close();*/

	ExtendedPart* singularPart = new ExtendedPart(Box(1.0, 2.0, 1.0), GlobalCFrame(7.0, 1.0, 5.0), {1.3, 1.2, 1.1}, "SingularPart");
	world.addPart(singularPart);

	ExtendedPart* ep1 = new ExtendedPart(Box(1.0, 2.0, 1.0), GlobalCFrame(3.0, 3.0, 0.0), { 1.0, 1.0, 1.0 }, "MainPart");
	ExtendedPart* ap1 = new ExtendedPart(Box(1.0, 2.0, 1.0), GlobalCFrame(), { 1.0, 1.0, 1.0 }, "AttachedPart");

	ep1->attach(ap1, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0.0, 0.0, 0.0));

	world.addPart(ep1);
	world.addPart(ap1);

	ep1->parent->mainPhysical->applyAngularImpulse(Vec3(1.0, 0.5, 0.0) * 1);

	ExtendedPart* ep1ap2 = new ExtendedPart(Box(0.5, 0.5, 0.5), ep1, CFrame(-0.5, 0.5, 0.5), {1.0, 1.0, 1.0});

	ExtendedPart* ep2 = new ExtendedPart(Box(1.0, 2.0, 1.0), GlobalCFrame(-3.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "MainPart");
	ExtendedPart* ap2 = new ExtendedPart(Box(1.0, 2.0, 1.0), GlobalCFrame(), {1.0, 1.0, 1.0}, "AttachedPart");

	ep2->attach(ap2, CFrame(1.0, 0.0, 0.0));

	world.addPart(ep2);
	world.addPart(ap2);

	ep2->parent->mainPhysical->applyAngularImpulse(Vec3(1.0, 0.5, 0.0) * 1);

	{
		ExtendedPart* fixedConstraintGroupMain = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(-3.0, 3.0, 3.0), {1.0, 1.0, 1.0}, "MainPart");
		ExtendedPart* f2 = new ExtendedPart(Box(0.9, 0.9, 0.9), GlobalCFrame(), {1.0, 1.0, 1.0}, "f2");
		ExtendedPart* f3 = new ExtendedPart(Box(0.8, 0.8, 0.8), GlobalCFrame(), {1.0, 1.0, 1.0}, "f3");
		ExtendedPart* f4 = new ExtendedPart(Box(0.7, 0.7, 0.7), GlobalCFrame(), {1.0, 1.0, 1.0}, "f4");
		ExtendedPart* f5 = new ExtendedPart(Box(0.6, 0.6, 0.6), GlobalCFrame(), {1.0, 1.0, 1.0}, "f5");

		fixedConstraintGroupMain->attach(f2, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f2->attach(f3, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f3->attach(f4, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f4->attach(f5, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));

		world.addPart(f2);
	}

	{
		ExtendedPart* nativeFixedConstraintGroupMain = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(-3.0, 3.0, 5.0), {1.0, 1.0, 1.0}, "MainPart");
		ExtendedPart* f2 = new ExtendedPart(Box(0.9, 0.9, 0.9), GlobalCFrame(), {1.0, 1.0, 1.0}, "f2");
		ExtendedPart* f3 = new ExtendedPart(Box(0.8, 0.8, 0.8), GlobalCFrame(), {1.0, 1.0, 1.0}, "f3");
		ExtendedPart* f4 = new ExtendedPart(Box(0.7, 0.7, 0.7), GlobalCFrame(), {1.0, 1.0, 1.0}, "f4");
		ExtendedPart* f5 = new ExtendedPart(Box(0.6, 0.6, 0.6), GlobalCFrame(), {1.0, 1.0, 1.0}, "f5");

		nativeFixedConstraintGroupMain->attach(f2, CFrame(1.2, 0.0, 0.0));
		f2->attach(f3, CFrame(1.2, 0.0, 0.0));
		f3->attach(f4, CFrame(1.2, 0.0, 0.0));
		f4->attach(f5, CFrame(1.2, 0.0, 0.0));

		world.addPart(f2);
	}

	{
		ExtendedPart* fixedConstraintGroupMain = new ExtendedPart(Box(1.0, 1.0, 1.0), GlobalCFrame(-3.0, 3.0, 7.0), {1.0, 1.0, 1.0}, "MainPart");
		ExtendedPart* f2 = new ExtendedPart(Box(0.9, 0.9, 0.9), GlobalCFrame(), {1.0, 1.0, 1.0}, "f2");
		ExtendedPart* f3 = new ExtendedPart(Box(0.8, 0.8, 0.8), GlobalCFrame(), {1.0, 1.0, 1.0}, "f3");
		ExtendedPart* f4 = new ExtendedPart(Box(0.7, 0.7, 0.7), GlobalCFrame(), {1.0, 1.0, 1.0}, "f4");
		ExtendedPart* f5 = new ExtendedPart(Box(0.6, 0.6, 0.6), GlobalCFrame(), {1.0, 1.0, 1.0}, "f5");

		f2->attach(f3, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		fixedConstraintGroupMain->attach(f2, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f3->attach(f4, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f4->attach(f5, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));

		world.addPart(f2);


		WorldImportExport::saveLooseParts("../testPart.parts", 1, &fixedConstraintGroupMain);
	}

	WorldImportExport::saveWorld("../testWorld.world", world);

	/*{
		int minX = -5;
		int maxX = 5;
		int minY = -5;
		int maxY = 5;
		int minZ = -5;
		int maxZ = 5;

		GlobalCFrame ref(0, 15, 0, fromEulerAngles(3.1415 / 4, 3.1415 / 4, 0.0));

		//Part* newCube = new Part(Box(1.0, 1.0, 1.0), ref.localToGlobal(CFrame(0,0,0)), {1.0, 0.2, 0.5});
		//world.addPart(newCube);

		for(double x = minX; x < maxX; x += 1.01) {
			for(double y = minY; y < maxY; y += 1.01) {
				for(double z = minZ; z < maxZ; z += 1.01) {
					ExtendedPart* newCube = new ExtendedPart(Library::createBox(1.0, 1.0, 1.0), ref.localToGlobal(CFrame(x, y, z)), {1.0, 0.2, 0.5});
					world.addPart(newCube);
				}
			}
		}
	}*/
}

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, [] () {
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
	if (paused) {
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
	world.asyncModification([] () {
		if (screen.camera.flying) {
			screen.camera.flying = false;
			screen.camera.attachment->setCFrame(GlobalCFrame(screen.camera.cframe.getPosition()));
			screen.world->addPart(screen.camera.attachment);
			screen.camera.attachment->parent->mainPhysical->momentResponse = SymmetricMat3::ZEROS();
		} else {
			screen.world->removePart(screen.camera.attachment);
			screen.camera.flying = true;
		}
		});
}

};

int main(int argc, const char** args) {
	Application::init(argc, args);

	Log::info("Started rendering");
	while (!Application::screen.shouldClose()) {
		graphicsMeasure.mark(GraphicsProcess::UPDATE);
		Application::screen.onUpdate();
		Application::screen.onRender();
		graphicsMeasure.end();
	}

	Application::stop(0);
}