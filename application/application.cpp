#include "core.h"

#include "application.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

#include "view/screen.h"
#include "ecs/material.h"
#include "ecs/entityBuilder.h"
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
#include "../physics/externalforces/gravityForce.h"
#include "../physics/misc/physicsProfiler.h"
#include "../physics/hardconstraints/motorConstraint.h"
#include "../physics/hardconstraints/sinusoidalPistonConstraint.h"
#include "../physics/hardconstraints/fixedConstraint.h"
#include "../physics/constraints/ballConstraint.h"
#include "../physics/constraints/hingeConstraint.h"
#include "../physics/constraints/barConstraint.h"
#include "../physics/softlinks/elasticLink.h"


#include "../physics/misc/serialization.h"

#include "worlds.h"
#include "tickerThread.h"
#include "worldBuilder.h"

#include "io/serialization.h"
#include "io/saveDialog.h"

#include "../util/resource/resourceManager.h"
#include "../util/parseCPUIDArgs.h"
#include "../util/cmdParser.h"
#include "../graphics/resource/textureResource.h"
#include "../graphics/meshRegistry.h"
#include "../engine/ecs/registry.h"
#include "../engine/event/keyEvent.h"
#include "../engine/input/keyboard.h"
#include "../engine/event/windowEvent.h"

#include "builtinWorlds.h"

#define TICKS_PER_SECOND 120.0
#define TICK_SKIP_TIME std::chrono::milliseconds(1000)

namespace P3D::Application {

TickerThread physicsThread;
PlayerWorld world(1 / TICKS_PER_SECOND);
Screen screen;

void init(const Util::ParsedArgs& cmdArgs);
void setupPhysics();
void setupWorld(const Util::ParsedArgs& cmdArgs);
void setupGL();
void setupDebug();

void loadFile(const char* file);

void init(const Util::ParsedArgs& cmdArgs) {
	auto start = high_resolution_clock::now();

	Log::init("latest.log");

	Log::info(Util::printAndParseCPUIDArgs(cmdArgs));
	bool quickBoot = cmdArgs.hasFlag("quickBoot");

	setupGL();

	Log::info("Init MeshRegistry");
	Graphics::MeshRegistry::init();

	/*ResourceManager::add<Graphics::TextureResource>("floorMaterial", "../res/textures/floor/floor_color.jpg");
	WorldImportExport::registerTexture(ResourceManager::get<Graphics::TextureResource>("floorMaterial"));*/

	Log::info("Initializing world");
	WorldBuilder::init();

	if(cmdArgs.argCount() >= 1) {
		loadFile(cmdArgs[0].c_str());
	} else {
		Log::info("Creating default world");
		setupWorld(cmdArgs);
	}

	Log::info("Initializing screen");
	screen.onInit(quickBoot);
	
	Log::info("Creating player");
	// Player
	screen.camera.attachment = new ExtendedPart(polyhedronShape(Library::createPrism(50, 0.3f, 1.5f)), GlobalCFrame(), {1.0, 5.0, 0.0}, "Player");

	if(!world.isValid()) {
		throw "World not valid!";
	}

	Log::info("Initializing physics");
	setupPhysics();
	Log::info("Initializing debug");
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

void setupWorld(const Util::ParsedArgs& cmdArgs) {
	Log::info("Initializing world");

	world.addExternalForce(new DirectionalGravity(Vec3(0, -10.0, 0.0)));

	PartProperties basicProperties{1.0, 0.7, 0.3};

	WorldBuilder::buildFloorAndWalls(50.0, 50.0, 1.0);


	// Lights
	{
		Comp::Light::Attenuation attenuation = {1, 1, 1};
		auto lights = EntityBuilder(screen.registry).name("Lights").get();
		auto sphereData = Graphics::MeshRegistry::getOrCreateMeshFor(&SphereClass::instance);
		EntityBuilder(screen.registry).parent(lights).transform(Position(10, 5, -10), 0.2).light(Color3(1, 0.84f, 0.69f), 300, attenuation).hitbox(&SphereClass::instance).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(10, 5, 10), 0.2).light(Color3(1, 0.84f, 0.69f), 300, attenuation).hitbox(&SphereClass::instance).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(-10, 5, -10), 0.2).light(Color3(1, 0.84f, 0.69f), 200, attenuation).hitbox(&SphereClass::instance).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(-10, 5, 10), 0.2).light(Color3(1, 0.84f, 0.69f), 500, attenuation).hitbox(&SphereClass::instance).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(0, 5, 0), 0.2).light(Color3(1, 0.90f, 0.75f), 400, attenuation).hitbox(&SphereClass::instance).mesh(sphereData);
	}
	{
		ExtendedPart* partA = new ExtendedPart(boxShape(1.0, 0.49, 3.0), GlobalCFrame(3.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partA");
		ExtendedPart* partB = new ExtendedPart(boxShape(1.0, 0.5, 3.0), GlobalCFrame(2.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partB");
		EntityBuilder(screen.registry, partA->entity).light(Color3(0.1, 0.94f, 0.49f), 500, Comp::Light::Attenuation{0.8, 0.5, 0.2});

		world.addPart(partA);
		world.addPart(partB);

		//world.addLink(new SpringLink({ CFrame{1.0, 0.0, 0.0}, partA }, { CFrame{0.0, 0.0, 0.0}, partB }, 5.0, 5.0));
		world.addLink(new ElasticLink({CFrame{1.0, 0.0, 0.0}, partA}, {CFrame{0.0, 0.0, 0.0}, partB}, 5.0, 5.0));
	}

	{
		ExtendedPart* partA = new ExtendedPart(boxShape(1.0, 0.49, 1.0), GlobalCFrame(12.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partA");
		ExtendedPart* partB = new ExtendedPart(boxShape(1.0, 0.5, 1.0), GlobalCFrame(14.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partB");

		ConstraintGroup cg;
		cg.add(partA, partB, new BarConstraint(Vec3(0.5, 0.0, 0.0), Vec3(-0.5, 0.0, 0.0), 1.0));
		world.constraints.push_back(cg);
		world.addPart(partA);
		world.addPart(partB);
	}

	//world.addLink(new MagneticLink({ CFrame{1.0, 0.0, 0.0}, partA }, { CFrame{0.0, 0.0, 0.0}, partB }, +8.0));
	//world.addLink(new AlignmentLink({ CFrame{1.0, 0.0, 0.0}, partA}, { CFrame{0.0, 0.0, 0.0}, partB }));
	return;
	
	/*ExtendedPart* partA = new ExtendedPart(boxShape(5.0, 10.0, 5.0), GlobalCFrame(0.0, 6.0, 0.0), WorldBuilder::basicProperties);
	ExtendedPart* partB = new ExtendedPart(sphereShape(0.45), GlobalCFrame(8.0, 6.0, 0.0), WorldBuilder::basicProperties);
	ExtendedPart* partC = new ExtendedPart(boxShape(0.9, 0.15, 0.15), GlobalCFrame(9.0, 6.0, 0.0), WorldBuilder::basicProperties);
	ExtendedPart* partD = new ExtendedPart(boxShape(0.9, 0.15, 0.15), GlobalCFrame(10.0, 6.0, 0.0), WorldBuilder::basicProperties);
	ExtendedPart* partE = new ExtendedPart(cylinderShape(0.3, 1.3), GlobalCFrame(11.0, 6.0, 0.0), WorldBuilder::basicProperties);

	world.createLayer(false, true);

	world.addPart(partA, 1);
	world.addPart(partB, 1);
	world.addPart(partC, 1);
	world.addPart(partD, 1);
	world.addPart(partE, 1);

	ConstraintGroup cg;
	cg.add(partA->parent, partB->parent, new BallConstraint(Vec3(7.0, 0.0, 0.0), Vec3(-1.0, 0.0, 0.0)));
	cg.add(partB->parent, partC->parent, new BallConstraint(Vec3(0.5, 0.0, 0.0), Vec3(-0.5, 0.0, 0.0)));
	cg.add(partC->parent, partD->parent, new BallConstraint(Vec3(0.5, 0.0, 0.0), Vec3(-0.5, 0.0, 0.0)));
	cg.add(partD->parent, partE->parent, new BallConstraint(Vec3(0.5, 0.0, 0.0), Vec3(-0.5, 0.0, 0.0)));
	world.constraints.push_back(std::move(cg));*/
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

	Util::ParsedArgs inputArgs(argc, args);

	init(inputArgs);

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