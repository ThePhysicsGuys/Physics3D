#include "core.h"

#include "application.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "view/screen.h"
#include "ecs/material.h"
#include "input/standardInputHandler.h"
#include "ecs/components.h"
#include "../graphics/texture.h"
#include "../graphics/debug/guiDebug.h"
#include "../graphics/debug/visualDebug.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/shapeCreation.h"
#include "../physics/math/mathUtil.h"
#include "../physics/math/linalg/commonMatrices.h"
#include "../physics/part.h"
#include "../physics/world.h"
#include "../physics/misc/gravityForce.h"
#include "../physics/physicsProfiler.h"
#include "../physics/constraints/motorConstraint.h"
#include "../physics/constraints/sinusoidalPistonConstraint.h"
#include "../physics/constraints/fixedConstraint.h"

#include "../physics/misc/serialization.h"

#include "worlds.h"
#include "tickerThread.h"
#include "worldBuilder.h"

#include "io/serialization.h"

#include "../util/resource/resourceManager.h"
#include "../graphics/resource/textureResource.h"
#include "../engine/meshRegistry.h"
#include "../engine/ecs/registry.h"
#include "../engine/event/keyEvent.h"
#include "../engine/input/keyboard.h"
#include "../engine/event/windowEvent.h"

#include "io/saveDialog.h"


#define TICKS_PER_SECOND 120.0
#define TICK_SKIP_TIME std::chrono::milliseconds(1000)

namespace P3D::Application {

TickerThread physicsThread;
PlayerWorld world(1 / TICKS_PER_SECOND);
Screen screen;

void init(int argc, const char** args);
void setupPhysics();
void setupWorld(int argc, const char** args);
void setupGL();
void setupDebug();

void loadFile(const char* file);

void init(int argc, const char** args) {
	auto start = high_resolution_clock::now();

	Log::init("latest.log");

	setupGL();
	Engine::MeshRegistry::init();

	ResourceManager::add<Graphics::TextureResource>("floorMaterial", "../res/textures/floor/floor_color.jpg");

	WorldImportExport::registerTexture(ResourceManager::get<Graphics::TextureResource>("floorMaterial"));

	WorldBuilder::init();

	if(argc >= 2) 
		loadFile(args[1]);
	else 
		setupWorld(argc, args);
	
	Log::info("Initializing screen");
	screen.onInit();
	
	// Player
	screen.camera.attachment = new ExtendedPart(polyhedronShape(Library::createPrism(50, 0.3f, 1.5f)), GlobalCFrame(), {1.0, 5.0, 0.0}, "Player");

	if(!world.isValid()) {
		throw "World not valid!";
	}

	setupPhysics();
	setupDebug();

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	Log::info("Init Physics3D in %.4f ms", duration.count() / 1.0f);
}

void setupGL() {
	Log::info("Initializing GLFW");
	if (!initGLFW()) {
		Log::error("GLFW not initialised");
		std::cin.get();
		stop(-1);
	}

	new(&screen) Screen(1800, 900, &world);

	Log::info("Initializing GLEW");
	if (!initGLEW()) {
		Log::error("GLEW not initialised");
		std::cin.get();
		stop(-1);
	}
}

void setupWorld(int argc, const char** args) {
	Log::info("Initializing world");

	world.addExternalForce(new DirectionalGravity(Vec3(0, -10.0, 0.0)));

	PartProperties basicProperties{1.0, 0.7, 0.3};

	// WorldBuilder init

	// Part factories
	WorldBuilder::SpiderFactory spiderFactories[] { {0.5, 4},{0.5, 6},{0.5, 8},{0.5, 10} };
	Shape triangle = polyhedronShape(Library::trianglePyramid);

	WorldBuilder::buildFloorAndWalls(50.0, 50.0, 1.0);
	

	{
		auto pistonFolder = screen.registry.create();
		screen.registry.add<Comp::Name>(pistonFolder, "PistonPart");
		
		ExtendedPart* centerPart = new ExtendedPart(sphereShape(1.0), GlobalCFrame(-15.0,4.0,13.0), basicProperties, "Center", pistonFolder);
		Shape box = boxShape(1.0, 1.0, 1.0);

		new ExtendedPart(Part(box, *centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::IDENTITY), CFrame(0.0, 0.0, 0.0), basicProperties), "IDENTITY", pistonFolder);
		new ExtendedPart(Part(box, *centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::X_90), CFrame(0.0, 0.0, 0.0), basicProperties), "X_90", pistonFolder);
		new ExtendedPart(Part(box, *centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::Y_90), CFrame(0.0, 0.0, 0.0), basicProperties), "Y_90", pistonFolder);
		new ExtendedPart(Part(box, *centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::X_180), CFrame(0.0, 0.0, 0.0), basicProperties), "X_180", pistonFolder);
		new ExtendedPart(Part(box, *centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::X_270), CFrame(0.0, 0.0, 0.0), basicProperties), "X_270", pistonFolder);
		new ExtendedPart(Part(box, *centerPart, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.0, 0.0, 0.0, Rotation::Predefined::Y_270), CFrame(0.0, 0.0, 0.0), basicProperties), "Y_270", pistonFolder);

		world.addPart(centerPart);
	}
	

	{
		ExtendedPart* sateliteBody = new ExtendedPart(cylinderShape(0.5, 1.0), GlobalCFrame(0.0, 7.0, 0.0, Rotation::Predefined::X_90), basicProperties, "Satelite Body");
		ExtendedPart* wing1 = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties, "Wing 1");
		ExtendedPart* wing2 = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties, "Wing 2");


		sateliteBody->attach(wing1, new SinusoidalPistonConstraint(0.0, 5.0, 1.0), CFrame(0.5, 0.0, 0.0, Rotation::Predefined::Y_90), CFrame(-0.5, 0.0, 0.0, Rotation::Predefined::Y_90));
		sateliteBody->attach(wing2, new SinusoidalPistonConstraint(0.0, 5.0, 1.0), CFrame(-0.5, 0.0, 0.0, Rotation::Predefined::Y_270), CFrame(-0.5, 0.0, 0.0, Rotation::Predefined::Y_90));
		//sateliteBody->attach(wing2, new SinusoidalPistonConstraint(Vec3(-1.0, 0.0, 0.0), 1.0, 3.0, 1.0), CFrame(-0.5, 0.0, 0.0), CFrame(0.5, 0.0, 0.0));

		world.addPart(sateliteBody);

		sateliteBody->parent->mainPhysical->motionOfCenterOfMass.rotation.rotation[0] = Vec3(0, 2, 0);
	}



	world.addPart(new ExtendedPart(sphereShape(2.0), GlobalCFrame(10, 3, 10), {1.0, 0.3, 0.7}, "SphereThing"));

	/*ExtendedPart* conveyor = new ExtendedPart(boxShape(1.0, 0.3, 50.0), GlobalCFrame(10.0, 0.65, 0.0), {2.0, 1.0, 0.3});

	conveyor->properties.conveyorEffect = Vec3(0, 0, 2.0);
	world.addTerrainPart(conveyor);

	world.addPart(new ExtendedPart(boxShape(0.2, 0.2, 0.2), GlobalCFrame(10, 1.0, 0.0), {1.0, 0.2, 0.3}, "TinyCube"));*/

	// hollow box
	WorldBuilder::HollowBoxParts parts = WorldBuilder::buildHollowBox(Bounds(Position(12.0, 3.0, 14.0), Position(20.0, 8.0, 20.0)), 0.3);

	parts.front->setMaterial(Comp::Material(Color(0.4f, 0.6f, 1.0f, 0.3f)));
	parts.back->setMaterial(Comp::Material(Color(0.4f, 0.6f, 1.0f, 0.3f)));

	// Rotating walls
	/*ExtendedPart* rotatingWall = new ExtendedPart(boxShape(5.0, 3.0, 0.5), GlobalCFrame(Position(-12.0, 1.7, 0.0)), {1.0, 1.0, 0.7});
	ExtendedPart* rotatingWall2 = new ExtendedPart(boxShape(5.0, 3.0, 0.5), GlobalCFrame(Position(-12.0, 1.7, 5.0)), {1.0, 1.0, 0.7});
	world.addPart(rotatingWall, true);
	world.addPart(rotatingWall2, true);
	rotatingWall->parent->mainPhysical->motionOfCenterOfMass.angularVelocity = Vec3(0, -0.7, 0);
	rotatingWall2->parent->mainPhysical->motionOfCenterOfMass.angularVelocity = Vec3(0, 0.7, 0);*/

	// Many many parts
	/*for(int i = 0; i < 3; i++) {
		ExtendedPart* newCube = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(fRand(-10.0, 0.0), fRand(1.0, 1.0), fRand(-10.0, 0.0)), {1.0, 0.2, 0.7});
		world.addPart(newCube);
	}*/

	WorldBuilder::buildCar(GlobalCFrame(5.0, 1.0, 5.0));


	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-10.0, 1.0, -10.0, Rotation::fromEulerAngles(0.15, 0.0, 0.0)), 1.5);
	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-12.5, 1.0, -14.0, Rotation::fromEulerAngles(0.0, 3.1415 / 2, -0.15)), 1.5);
	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-16.5, 1.0, -11.5, Rotation::fromEulerAngles(-0.15, 0.0, -0.0)), -1.5);
	WorldBuilder::buildConveyor(1.5, 7.0, GlobalCFrame(-14.0, 1.0, -7.5, Rotation::fromEulerAngles(0.0, 3.1415 / 2, 0.15)), -1.5);

	int minX = 0;
	int maxX = 3;
	int minY = 0;
	int maxY = 3;
	int minZ = 0;
	int maxZ = 3;

	auto cubeFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(cubeFolder, "Cubes");
	auto cylinderFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(cylinderFolder, "Cylinders");
	auto triangleFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(triangleFolder, "Triangles");
	auto sphereFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(sphereFolder, "Spheres");
	auto spiderFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(spiderFolder, "Spiders");
	
	GlobalCFrame rootFrame(Position(0.0, 15.0, 0.0), Rotation::fromEulerAngles(3.1415 / 4, 3.1415 / 4, 0.0));
	for (double x = minX; x < maxX; x += 1.00001) {
		for (double y = minY; y < maxY; y += 1.00001) {
			for (double z = minZ; z < maxZ; z += 1.00001) {
				ExtendedPart* newCube = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(Position(x - 5, y + 10, z - 5)), { 1.0, 1.0, 0.0 }, "Box", cubeFolder);
				newCube->setMaterial(Comp::Material(Vec4f(float((x - minX) / (maxX - minX)), float((y - minY) / (maxY - minY)), float((z - minZ) / (maxZ - minZ)), 1.0f)));

				world.addPart(newCube);
				world.addPart(new ExtendedPart(sphereShape(0.5), GlobalCFrame(Position(x + 5, y + 1, z - 5)), { 1.0, 0.2, 0.5 }, "Sphere", sphereFolder));
				spiderFactories[rand() & 0x00000003].buildSpider(GlobalCFrame(Position(x+y*0.1, y+1, z)), spiderFolder);
				world.addPart(new ExtendedPart(triangle, GlobalCFrame(Position(x - 20, y + 1, z + 20)), { 1.0, 0.2, 0.5 }, "Triangle", triangleFolder));

				world.addPart(new ExtendedPart(cylinderShape(0.3, 1.2), GlobalCFrame(x - 5, y + 1, z + 5, Rotation::fromEulerAngles(3.1415/4, 3.1415/4, 0.0)), {1.0, 0.2, 0.5}, "cylinderShape", cylinderFolder));
			}
		}
	}

	//auto terrainFolder = screen.registry.create();
	//screen.registry.add<Comp::Tag>(terrainFolder, "Terrain");
	//WorldBuilder::buildTerrain(150, 150, terrainFolder);

	auto ropeFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(ropeFolder, "Ropes");
	ExtendedPart* ropeA = new ExtendedPart(boxShape(2.0, 1.5, 0.7), GlobalCFrame(10.0, 2.0, -10.0), {1.0, 0.7, 0.3}, "RopeA", ropeFolder);
	ExtendedPart* ropeB = new ExtendedPart(boxShape(1.5, 1.2, 0.9), GlobalCFrame(10.0, 2.0, -14.0), {1.0, 0.7, 0.3}, "RopeB", ropeFolder);
	ExtendedPart* ropeC = new ExtendedPart(boxShape(2.0, 1.5, 0.7), GlobalCFrame(10.0, 2.0, -18.0), {1.0, 0.7, 0.3}, "RopeC", ropeFolder);

	world.addPart(ropeA);
	world.addPart(ropeB);
	world.addPart(ropeC);

	ConstraintGroup group;

	group.add(ropeA->parent, ropeB->parent, new BallConstraint(Vec3(0.0, 0.0, -2.0), Vec3(0.0, 0.0, 2.0)));
	group.add(ropeB->parent, ropeC->parent, new BallConstraint(Vec3(0.0, 0.0, -2.0), Vec3(0.0, 0.0, 2.0)));

	world.constraints.push_back(group);

	ExtendedPart* singularPart = new ExtendedPart(boxShape(1.0, 2.0, 1.0), GlobalCFrame(7.0, 1.0, 5.0), {1.3, 1.2, 1.1}, "SingularPart");
	world.addPart(singularPart);

	ExtendedPart* ep1 = new ExtendedPart(boxShape(1.0, 2.0, 1.0), GlobalCFrame(3.0, 3.0, 0.0), { 1.0, 1.0, 1.0 }, "MainPart");
	ExtendedPart* ap1 = new ExtendedPart(boxShape(1.0, 2.0, 1.0), GlobalCFrame(), { 1.0, 1.0, 1.0 }, "AttachedPart");

	ep1->attach(ap1, new FixedConstraint(), CFrame(1.0, 0.0, 0.0), CFrame(0.0, 0.0, 0.0));

	world.addPart(ep1);
	world.addPart(ap1);

	ep1->parent->mainPhysical->applyAngularImpulse(Vec3(1.0, 0.5, 0.0) * 1);

	ExtendedPart* ep1ap2 = new ExtendedPart(boxShape(0.5, 0.5, 0.5), ep1, CFrame(-0.5, 0.5, 0.5), {1.0, 1.0, 1.0}, "Ep1Ap2");
	ExtendedPart* ep2 = new ExtendedPart(boxShape(1.0, 2.0, 1.0), GlobalCFrame(-3.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "MainPart");
	ExtendedPart* ap2 = new ExtendedPart(boxShape(1.0, 2.0, 1.0), GlobalCFrame(), {1.0, 1.0, 1.0}, "AttachedPart");

	ep2->attach(ap2, CFrame(1.0, 0.0, 0.0));

	world.addPart(ep2);
	world.addPart(ap2);

	ep2->parent->mainPhysical->applyAngularImpulse(Vec3(1.0, 0.5, 0.0) * 1);

	//SoftLink.
	{
		ExtendedPart* a = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(3.0, 3.0, 0.0), { 1.0, 1.0, 1.0 }, "SpringLinkMain1");
		ExtendedPart* b = new ExtendedPart(boxShape(1.0, 2.0, 1.0), GlobalCFrame(), { 1.0, 1.0, 1.0 }, "SpringLinkPart1");

		world.addPart(a);
		world.addPart(b);

		world.addLink(new SpringLink( {CFrame{1.0, 0.0, 0.0}, a}, {CFrame{0.0, 0.0, 0.0}, b}, 15.0, 0.5));
	}
	{
		ExtendedPart* a = new ExtendedPart(boxShape(2.0, 0.5, 1.0), GlobalCFrame(3.0, 3.0, 0.0), { 1.0, 1.0, 1.0 }, "SpringLinkMain2");
		ExtendedPart* b = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), { 2.0, 1.0, 1.0 }, "SpringLinkPart2");

		world.addPart(a);
		world.addPart(b);

		world.addLink(new SpringLink({ CFrame{1.0, 0.0, 0.0}, a }, { CFrame{0.0, 0.0, 0.0}, b }, 15.0, 0.5));
	}
	{
		ExtendedPart* a = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(3.0, 3.0, 0.0), { 1.0, 1.0, 1.0 }, "SpringLinkMain3");
		ExtendedPart* b = new ExtendedPart(boxShape(2.0, 2.0, 2.0), GlobalCFrame(), { 1.0, 0.5, 1.0 }, "SpringLinkPart3");

		world.addPart(a);
		world.addPart(b);

		world.addLink(new SpringLink({ CFrame{1.0, 0.0, 0.0}, a }, { CFrame{0.0, 0.0, 0.0}, b }, 15.0, 0.5));
	}
	
	/*Vec3 angularVel(0.0, 0.0, -1.0);
	{
		ExtendedPart* nativeFixedConstraintGroupMain = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(Position(-3.0, 7.0, 2.0), Rotation::fromEulerAngles(0.0, 0.0, -0.5)), {1.0, 1.0, 1.0}, "MainPart");
		ExtendedPart* f2 = new ExtendedPart(boxShape(0.9, 0.9, 0.9), GlobalCFrame(), {1.0, 1.0, 1.0}, "f2");
		ExtendedPart* f3 = new ExtendedPart(boxShape(0.8, 0.8, 0.8), GlobalCFrame(), {1.0, 1.0, 1.0}, "f3");
		ExtendedPart* f4 = new ExtendedPart(boxShape(0.7, 0.7, 0.7), GlobalCFrame(), {1.0, 1.0, 1.0}, "f4");
		ExtendedPart* f5 = new ExtendedPart(boxShape(0.6, 0.6, 0.6), GlobalCFrame(), {1.0, 1.0, 1.0}, "f5");

		nativeFixedConstraintGroupMain->attach(f2, CFrame(1.2, 0.0, 0.0));
		f2->attach(f3, CFrame(1.2, 0.0, 0.0));
		f3->attach(f4, CFrame(1.2, 0.0, 0.0));
		f4->attach(f5, CFrame(1.2, 0.0, 0.0));

		world.addPart(f2);

		f2->parent->mainPhysical->motionOfCenterOfMass.rotation.angularVelocity = angularVel;
	}

	{
		ExtendedPart* fixedConstraintGroupMain = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(Position(-3.0, 7.0, -2.0), Rotation::fromEulerAngles(0.0, 0.0, -0.5)), {1.0, 1.0, 1.0}, "MainPart");
		ExtendedPart* f2 = new ExtendedPart(boxShape(0.9, 0.9, 0.9), GlobalCFrame(), {1.0, 1.0, 1.0}, "f2");
		ExtendedPart* f3 = new ExtendedPart(boxShape(0.8, 0.8, 0.8), GlobalCFrame(), {1.0, 1.0, 1.0}, "f3");
		ExtendedPart* f4 = new ExtendedPart(boxShape(0.7, 0.7, 0.7), GlobalCFrame(), {1.0, 1.0, 1.0}, "f4");
		ExtendedPart* f5 = new ExtendedPart(boxShape(0.6, 0.6, 0.6), GlobalCFrame(), {1.0, 1.0, 1.0}, "f5");

		fixedConstraintGroupMain->attach(f2, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f2->attach(f3, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f3->attach(f4, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f4->attach(f5, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));

		world.addPart(f2);

		f2->parent->mainPhysical->motionOfCenterOfMass.rotation.angularVelocity = angularVel;
	}*/

	{
		ExtendedPart* fixedConstraintGroupMain = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(-3.0, 3.0, 7.0), {1.0, 1.0, 1.0}, "MainPart");
		ExtendedPart* f2 = new ExtendedPart(boxShape(0.9, 0.9, 0.9), GlobalCFrame(), {1.0, 1.0, 1.0}, "f2");
		ExtendedPart* f3 = new ExtendedPart(boxShape(0.8, 0.8, 0.8), GlobalCFrame(), {1.0, 1.0, 1.0}, "f3");
		ExtendedPart* f4 = new ExtendedPart(boxShape(0.7, 0.7, 0.7), GlobalCFrame(), {1.0, 1.0, 1.0}, "f4");
		ExtendedPart* f5 = new ExtendedPart(boxShape(0.6, 0.6, 0.6), GlobalCFrame(), {1.0, 1.0, 1.0}, "f5");

		f2->attach(f3, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		fixedConstraintGroupMain->attach(f2, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f3->attach(f4, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));
		f4->attach(f5, new FixedConstraint(), CFrame(1.2, 0.0, 0.0), CFrame(0, 0, 0));

		world.addPart(f2);
	}


	{
		double turnSpeed = 10.0;

		auto poweredCarFolder = screen.registry.create();
		screen.registry.add<Comp::Name>(poweredCarFolder, "Powered Car");
		
		ExtendedPart* poweredCarBody = new ExtendedPart(boxShape(1.0, 0.4, 2.0), GlobalCFrame(-6.0, 1.0, 0.0), basicProperties, "Chassis", poweredCarFolder);
		ExtendedPart* FLWheel = new ExtendedPart(cylinderShape(0.5, 0.2), GlobalCFrame(), basicProperties, "Front Left Wheel", poweredCarFolder);
		ExtendedPart* FRWheel = new ExtendedPart(cylinderShape(0.5, 0.2), GlobalCFrame(), basicProperties, "Front Right Wheel", poweredCarFolder);
		ExtendedPart* BLWheel = new ExtendedPart(cylinderShape(0.5, 0.2), GlobalCFrame(), basicProperties, "Back Left Wheel", poweredCarFolder);
		ExtendedPart* BRWheel = new ExtendedPart(cylinderShape(0.5, 0.2), GlobalCFrame(), basicProperties, "Back Right Wheel", poweredCarFolder);

		poweredCarBody->attach(FLWheel, new ConstantSpeedMotorConstraint(turnSpeed), CFrame(Vec3(0.55, 0.0, 1.0), Rotation::Predefined::Y_90), CFrame(Vec3(0.0, 0.0, 0.15), Rotation::Predefined::Y_180));
		poweredCarBody->attach(BLWheel, new ConstantSpeedMotorConstraint(turnSpeed), CFrame(Vec3(0.55, 0.0, -1.0), Rotation::Predefined::Y_90), CFrame(Vec3(0.0, 0.0, 0.15), Rotation::Predefined::Y_180));
		poweredCarBody->attach(FRWheel, new ConstantSpeedMotorConstraint(-turnSpeed), CFrame(Vec3(-0.55, 0.0, 1.0), Rotation::Predefined::Y_270), CFrame(Vec3(0.0, 0.0, 0.15), Rotation::Predefined::Y_180));
		poweredCarBody->attach(BRWheel, new ConstantSpeedMotorConstraint(-turnSpeed), CFrame(Vec3(-0.55, 0.0, -1.0), Rotation::Predefined::Y_270), CFrame(Vec3(0.0, 0.0, 0.15), Rotation::Predefined::Y_180));

		world.addPart(poweredCarBody);

		//poweredCarBody->parent->mainPhysical->motionOfCenterOfMass.angularVelocity = Vec3(0.0, 1.0, 0.0);
	}

	{
		ExtendedPart* mainBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0.0, 5.0, 5.0), basicProperties, "Main Block");
		ExtendedPart* attachedBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties, "Attached Block");

		mainBlock->attach(attachedBlock, new ConstantSpeedMotorConstraint(1.0), CFrame(Vec3(0.0, 0.0, 0.5)), CFrame(Vec3(1.0, 0.0, -0.5)));

		world.addPart(mainBlock);
	}

	// needed for conservation of angular momentum
	{
		ExtendedPart* mainBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0.0, 5.0, -5.0), basicProperties, "Main Block");
		ExtendedPart* attachedBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties, "Attached Block");

		mainBlock->attach(attachedBlock, new MotorConstraintTemplate<SineWaveController>(0.0, 6.283, 1.0), CFrame(Vec3(0.0, 0.0, 0.5)), CFrame(Vec3(0.0, 0.0, -0.5)));

		world.addPart(mainBlock);
	}

	{
		ExtendedPart* mainBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(0.0, 5.0, 10.0), basicProperties, "Main Block");
		ExtendedPart* attachedBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties, "Attached Block");
		ExtendedPart* anotherAttachedBlock = new ExtendedPart(boxShape(1.0, 1.0, 1.0), GlobalCFrame(), basicProperties, "Another Attached Block");
		ExtendedPart* attachedCylinder = new ExtendedPart(cylinderShape(0.5, 1.0), GlobalCFrame(), basicProperties, "Rotating Attached Block");
		ExtendedPart* attachedBall = new ExtendedPart(sphereShape(0.5), GlobalCFrame(), basicProperties, "Attached Ball");

		mainBlock->attach(attachedBlock, new SinusoidalPistonConstraint(1.0, 3.0, 1.0), CFrame(0.5, 0.0, 0.0, Rotation::Predefined::Y_90), CFrame(-0.5, 0.0, 0.0, Rotation::Predefined::Y_90));
		attachedBlock->attach(anotherAttachedBlock, new SinusoidalPistonConstraint(1.0, 2.0, 0.7), CFrame(0.0, 0.5, 0.0, Rotation::Predefined::X_270), CFrame(0.0, -0.5, 0.0, Rotation::Predefined::X_270));

		attachedBlock->attach(attachedCylinder, new ConstantSpeedMotorConstraint(3.0), CFrame(0.0, 0.0, 0.55), CFrame(0.0, 0.0, -0.55));

		attachedCylinder->attach(attachedBall, new SinusoidalPistonConstraint(0.0, 2.0, 0.7), CFrame(0.0, 0.5, 0.0, Rotation::Predefined::X_270), CFrame(0.0, -0.5, 0.0, Rotation::Predefined::X_270));

		world.addPart(mainBlock);

		mainBlock->parent->mainPhysical->motionOfCenterOfMass.rotation.rotation[0] = Vec3(0, 2, 0);
	}

	{
		ExtendedPart* box1 = new ExtendedPart(boxShape(2.0, 1.0, 1.0), GlobalCFrame(-10.0, 5.0, 10.0), basicProperties, "Box1");
		ExtendedPart* box2 = new ExtendedPart(boxShape(2.0, 1.0, 1.0), GlobalCFrame(-11.1, 5.0, 11.9, Rotation::rotY(3.1415 * 2 / 3)), basicProperties, "Box2");
		ExtendedPart* box3 = new ExtendedPart(boxShape(2.0, 1.0, 1.0), GlobalCFrame(-8.9, 5.0, 11.9, Rotation::rotY(-3.1415 * 2 / 3)), basicProperties, "Box3");

		world.addPart(box1);
		world.addPart(box2);
		world.addPart(box3);

		ConstraintGroup group;

		group.add(box1->parent, box3->parent, new BallConstraint(Vec3(1.0, 0.0, 0.7), Vec3(-1.0, 0.0, 0.7)));
		group.add(box2->parent, box1->parent, new BallConstraint(Vec3(1.0, 0.0, 0.7), Vec3(-1.0, 0.0, 0.7)));
		group.add(box3->parent, box2->parent, new BallConstraint(Vec3(1.0, 0.0, 0.7), Vec3(-1.0, 0.0, 0.7)));

		world.constraints.push_back(std::move(group));
	}

	Shape torusShape = polyhedronShape(Library::createTorus(1.0f, 0.6f, 80, 80));
	Engine::MeshRegistry::registerMeshFor(torusShape.baseShape, Graphics::VisualShape::generateSmoothNormalsShape(torusShape.baseShape->asPolyhedron()));
	world.addPart(new ExtendedPart(torusShape, Position(-10.0, 3.0, 0.0), basicProperties, "Torus"));


	Vec2f toyPoints[]{{0.2f, 0.2f},{0.3f, 0.4f},{0.2f, 0.6f},{0.3f, 0.8f},{0.4f,0.7f},{0.5f,0.4f},{0.6f,0.2f},{0.75f,0.1f},{0.9f,0.015f}};
	Shape toyShape = polyhedronShape(Library::createRevolvedShape(0.0f, toyPoints, 9, 1.0f, 10));
	world.addPart(new ExtendedPart(toyShape, Position(-10.0, 3.0, 3.0), basicProperties, "ToyPoints"));

	Vec2f arrowPoints[]{{0.3f,0.1f},{0.3f,0.04f},{1.0f,0.04f}};
	Shape arrorShape = polyhedronShape(Library::createRevolvedShape(0.0f, arrowPoints, 3, 1.0f, 40));
	world.addPart(new ExtendedPart(arrorShape, Position(-7.0, 3.0, 0.0), basicProperties, "ArrowPoints"));
}

void setupPhysics() {
	physicsThread = TickerThread(TICKS_PER_SECOND, TICK_SKIP_TIME, [] () {
		physicsMeasure.mark(PhysicsProcess::OTHER);

		Graphics::AppDebug::logTickStart();
		world.tick();
		Graphics::AppDebug::logTickEnd();

		physicsMeasure.end();

		GJKCollidesIterationStatistics.nextTally();
		GJKNoCollidesIterationStatistics.nextTally();
		EPAIterationStatistics.nextTally();
	});
}

void setupDebug() {
	Graphics::AppDebug::setupDebugHooks();
}

void loadFile(const char* file) {
	Log::info("Loading file %s", file);
	auto startTime = high_resolution_clock::now();
	if(Util::endsWith(file, ".parts")) {
		WorldImportExport::loadLoosePartsIntoWorld(file, world);
	} else if(Util::endsWith(file, ".nativeParts")) {
		WorldImportExport::loadNativePartsIntoWorld(file, world);
	} else if(Util::endsWith(file, ".world")) {
		WorldImportExport::loadWorld(file, world);
	}
	nanoseconds deltaTime = high_resolution_clock::now() - startTime;
	Log::info("File loaded, took %.4f ms", deltaTime.count() / 1E6);
}

bool onFileDrop(Engine::WindowDropEvent& event) {
	loadFile(event.getPath().c_str());

	return true;
}

bool onKeyPress(Engine::KeyPressEvent& keyEvent) {
	using namespace Engine;

	Key pressedKey = keyEvent.getKey();
	if(pressedKey == Keyboard::KEY_S && keyEvent.getModifiers().isCtrlPressed()) {
		handler->setKey(Keyboard::KEY_S, false);
		saveWorld(world);
		return true;
	} else if(pressedKey == Keyboard::KEY_O && keyEvent.getModifiers().isCtrlPressed()) {
		handler->setKey(Keyboard::KEY_O, false);
		openWorld(world);
		return true;
	} else {
		return false;
	}
}

void onEvent(Engine::Event& event) {
	using namespace Engine;

	screen.onEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<WindowDropEvent>(onFileDrop);
	dispatcher.dispatch<KeyPressEvent>(onKeyPress);
}

void stop(int returnCode) {
	Log::info("Closing physics");
	physicsThread.stop();

	Log::info("Closing screen");
	screen.onClose();

	Log::stop();
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
	using namespace P3D::Application;
	using namespace P3D::Graphics;

	init(argc, args);

	Log::info("Started rendering");
	while (!screen.shouldClose()) {
		graphicsMeasure.mark(GraphicsProcess::UPDATE);

		screen.onUpdate();
		screen.onRender();

		graphicsMeasure.end();
	}

	Log::info("Closing by screen.shouldClose()");

	stop(0);
}