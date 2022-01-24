#include "core.h"

#include "application.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "view/screen.h"
#include "ecs/entityBuilder.h"
#include "input/standardInputHandler.h"
#include "ecs/components.h"
#include "../graphics/texture.h"
#include "../graphics/ecs/components.h"
#include "../graphics/debug/guiDebug.h"
#include "../graphics/debug/visualDebug.h"
#include <Physics3D/geometry/shapeCreation.h>
#include <Physics3D/math/linalg/commonMatrices.h>
#include <Physics3D/part.h>
#include <Physics3D/world.h>
#include <Physics3D/externalforces/directionalGravity.h>
#include <Physics3D/misc/physicsProfiler.h>
#include <Physics3D/hardconstraints/motorConstraint.h>
#include <Physics3D/hardconstraints/sinusoidalPistonConstraint.h>
#include <Physics3D/hardconstraints/fixedConstraint.h>
#include <Physics3D/constraints/ballConstraint.h>
#include <Physics3D/constraints/hingeConstraint.h>
#include <Physics3D/constraints/barConstraint.h>
#include <Physics3D/softlinks/elasticLink.h>

#include <Physics3D/misc/serialization/serialization.h>
#include <Physics3D/threading/physicsThread.h>

#include "worlds.h"
#include "worldBuilder.h"

#include "io/serialization.h"
#include "io/saveDialog.h"

#include "../util/parseCPUIDArgs.h"
#include "../util/cmdParser.h"
#include "../graphics/meshRegistry.h"
#include "../engine/event/keyEvent.h"
#include "../engine/input/keyboard.h"
#include "../engine/event/windowEvent.h"

#include "../util/stringUtil.h"

#include <Physics3D/misc/toString.h>

#include "graphics/resource/textureResource.h"
#include "util/resource/resourceManager.h"


#define TICKS_PER_SECOND 120.0
#define TICK_SKIP_TIME std::chrono::milliseconds(1000)

namespace P3D::Application {

PlayerWorld world(1 / TICKS_PER_SECOND);
UpgradeableMutex worldMutex;
PhysicsThread physicsThread(&world, &worldMutex, Graphics::AppDebug::logTickEnd, TICK_SKIP_TIME);
Screen screen;

void init(const ::Util::ParsedArgs& cmdArgs);
void setupWorld(const ::Util::ParsedArgs& cmdArgs);
void setupGL();
void setupDebug();

void loadFile(const char* file);

void init(const ::Util::ParsedArgs& cmdArgs) {
	auto start = std::chrono::high_resolution_clock::now();

	Log::init("latest.log");

	Log::info(::Util::printAndParseCPUIDArgs(cmdArgs));
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
	
	if(!world.isValid()) {
		throw "World not valid!";
	}

	Log::info("Initializing debug");
	setupDebug();

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	Log::info("Init Physics3D in %.4f ms", duration.count() / 1.0f);
}

void setupGL() {
	Log::info("Initializing GLFW");
	if (!initGLFW()) {
		Log::error("GLFW not initialised");
		std::cin.get();
		stop(-1);
	}

	new(&screen) Screen(1800, 900, &world, &worldMutex);

	Log::info("Initializing GLEW");
	if (!initGLEW()) {
		Log::error("GLEW not initialised");
		std::cin.get();
		stop(-1);
	}

	std::filesystem::copy_file("../res/default_imgui.ini", "../res/imgui.ini", std::filesystem::copy_options::skip_existing);
}

void setupWorld(const ::Util::ParsedArgs& cmdArgs) {
	Log::info("Initializing world");

	world.addExternalForce(new DirectionalGravity(Vec3(0, -10.0, 0.0)));


	//buildShowcaseWorld(screen, world); return;

	PartProperties basicProperties{1.0, 0.7, 0.3};

	WorldBuilder::buildFloorAndWalls(50.0, 50.0, 1.0);
	
	GlobalCFrame origin(0.0, 5.0, 0.0, Rotation::fromEulerAngles(-3.14 / 4, 3.14 / 4, 0.0));

	// Load textures
	/*Graphics::TextureResource* wallAlbedo = ResourceManager::add<Graphics::TextureResource>("wall albedo", "../res/textures/wall/wall_color.jpg");
	Graphics::TextureResource* wallNormal = ResourceManager::add<Graphics::TextureResource>("wall normal", "../res/textures/wall/wall_normal.jpg");*/

	int n = 3;
	for(int x = 0; x < n; x++) {
		for(int y = 0; y < n; y++) {
			for(int z = 0; z < n; z++) {
				GlobalCFrame cf = origin.localToGlobal(CFrame(x, y, z));
				std::string name = "part " + std::to_string((x * n + y) * n + z);
				ExtendedPart* part = new ExtendedPart(boxShape(0.5, 0.5, 0.5), cf, basicProperties, name);

				/*IRef<Comp::Material> material = screen.registry.add<Comp::Material>(part->entity);
				material->set(Comp::Material::ALBEDO, SRef<Graphics::Texture>(wallAlbedo));
				material->set(Comp::Material::NORMAL, SRef<Graphics::Texture>(wallNormal));*/

				world.addPart(part);
			}
		}
	}
	
	{
		ExtendedPart* partA = new ExtendedPart(boxShape(1.0, 0.49, 3.0), GlobalCFrame(3.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partA");
		ExtendedPart* partB = new ExtendedPart(boxShape(1.0, 0.5, 3.0), GlobalCFrame(2.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partB");
		world.addPart(partA);
		partA->attach(partB, CFrame(0.0, 1.0, 0.0));
	}
	{
		ExtendedPart* partA = new ExtendedPart(boxShape(1.0, 0.49, 3.0), GlobalCFrame(-3.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partA");
		ExtendedPart* partB = new ExtendedPart(boxShape(1.0, 0.5, 3.0), GlobalCFrame(-2.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partB");
		world.addPart(partA);
		partA->attach(partB, new MotorConstraintTemplate<ConstantMotorTurner>(0.5), CFrame(0.0, 1.0, 0.0), CFrame(1.0, 0.0, 0.0));
	}
	
	// Lights
	{
		Comp::Light::Attenuation attenuation = {1, 1, 1};
		auto lights = EntityBuilder(screen.registry).name("Lights").get();
		auto sphereData = Graphics::MeshRegistry::getMesh(&SphereClass::instance);
		EntityBuilder(screen.registry).parent(lights).transform(Position(10, 5, -10), 0.2).light(Graphics::Color(1, 0.84f, 0.69f), 300, attenuation).hitbox(sphereShape(1.0)).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(10, 5, 10), 0.2).light(Graphics::Color(1, 0.84f, 0.69f), 300, attenuation).hitbox(sphereShape(1.0)).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(-10, 5, -10), 0.2).light(Graphics::Color(1, 0.84f, 0.69f), 200, attenuation).hitbox(sphereShape(1.0)).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(-10, 5, 10), 0.2).light(Graphics::Color(1, 0.84f, 0.69f), 500, attenuation).hitbox(sphereShape(1.0)).mesh(sphereData);
		EntityBuilder(screen.registry).parent(lights).transform(Position(0, 5, 0), 0.2).light(Graphics::Color(1, 0.90f, 0.75f), 400, attenuation).hitbox(sphereShape(1.0)).mesh(sphereData);
	}
	{
		ExtendedPart* cornerPart = new ExtendedPart(cornerShape(2.0, 2.0, 2.0), GlobalCFrame(3.0, 3.0, -5.0), {1.0, 1.0, 1.0}, "CORNER");
		ExtendedPart* wedgePart = new ExtendedPart(wedgeShape(2.0, 2.0, 2.0), GlobalCFrame(-3.0, 3.0, -5.0), {1.0, 1.0, 1.0}, "WEDGE");
		world.addPart(cornerPart);
		world.addPart(wedgePart);
	}

	{
		ExtendedPart* partA = new ExtendedPart(boxShape(1.0, 0.49, 3.0), GlobalCFrame(3.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partA");
		ExtendedPart* partB = new ExtendedPart(boxShape(1.0, 0.5, 3.0), GlobalCFrame(2.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partB");
		EntityBuilder(screen.registry, partA->entity).light(Graphics::Color(0.1f, 0.94f, 0.49f), 500, Comp::Light::Attenuation{0.8, 0.5, 0.2});

		world.addPart(partA);
		world.addPart(partB);

		//world.addLink(new SpringLink({ CFrame{1.0, 0.0, 0.0}, partA }, { CFrame{0.0, 0.0, 0.0}, partB }, 5.0, 5.0));
		world.addLink(new ElasticLink({CFrame{1.0, 0.0, 0.0}, partA}, {CFrame{0.0, 0.0, 0.0}, partB}, 5.0, 5.0));
	}

	{
		ExtendedPart* partA = new ExtendedPart(boxShape(1.0, 0.49, 1.0), GlobalCFrame(12.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partA");
		ExtendedPart* partB = new ExtendedPart(boxShape(1.0, 0.5, 1.0), GlobalCFrame(14.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partB");
		ExtendedPart* partC = new ExtendedPart(boxShape(1.0, 0.5, 1.0), GlobalCFrame(16.0, 3.0, 0.0), {1.0, 1.0, 1.0}, "partC");

		ConstraintGroup cg;
		cg.add(partA, partB, new BarConstraint(Vec3(0.5, 0.0, 0.0), Vec3(-0.5, 0.0, 0.0), 1.0));
		cg.add(partB, partC, new BallConstraint(Vec3(1, 0.0, 0.0), Vec3(-1, 0.0, 0.0)));
		cg.add(partC, partA, new HingeConstraint(Vec3(-2, 2.0, 0.0), Vec3(0,0,1), Vec3(2, 2.0, 0.0), Vec3(0, 0, 1)));
		world.constraints.push_back(cg);
		world.addPart(partA);
		world.addPart(partB);
		world.addPart(partC);
	}

	ExtendedPart* tetrahedron = new ExtendedPart(polyhedronShape(ShapeLibrary::tetrahedron), GlobalCFrame(-7.0, 3.0, 6.0), WorldBuilder::basicProperties);
	ExtendedPart* octahedron = new ExtendedPart(polyhedronShape(ShapeLibrary::octahedron), GlobalCFrame(-7.0, 3.0, 8.0), WorldBuilder::basicProperties);
	ExtendedPart* icosahedron = new ExtendedPart(polyhedronShape(ShapeLibrary::icosahedron), GlobalCFrame(-7.0, 3.0, 10.0), WorldBuilder::basicProperties);

	world.addPart(tetrahedron);
	world.addPart(octahedron);
	world.addPart(icosahedron);

	//world.addLink(new MagneticLink({ CFrame{1.0, 0.0, 0.0}, partA }, { CFrame{0.0, 0.0, 0.0}, partB }, +8.0));
	//world.addLink(new AlignmentLink({ CFrame{1.0, 0.0, 0.0}, partA}, { CFrame{0.0, 0.0, 0.0}, partB }));
	//return;
	
	ExtendedPart* partA = new ExtendedPart(boxShape(5.0, 10.0, 5.0), GlobalCFrame(0.0, 6.0, -10.0), WorldBuilder::basicProperties);
	ExtendedPart* partB = new ExtendedPart(sphereShape(0.45), GlobalCFrame(8.0, 6.0, -10.0), WorldBuilder::basicProperties);
	ExtendedPart* partC = new ExtendedPart(boxShape(0.9, 0.15, 0.15), GlobalCFrame(9.0, 6.0, -10.0), WorldBuilder::basicProperties);
	ExtendedPart* partD = new ExtendedPart(boxShape(0.9, 0.15, 0.15), GlobalCFrame(10.0, 6.0, -10.0), WorldBuilder::basicProperties);
	ExtendedPart* partE = new ExtendedPart(cylinderShape(0.3, 1.3), GlobalCFrame(11.0, 6.0, -10.0), WorldBuilder::basicProperties);

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
	world.constraints.push_back(std::move(cg));
}

void setupDebug() {
	Graphics::AppDebug::setupDebugHooks();
}

void loadFile(const char* file) {
	Log::info("Loading file %s", file);
	auto startTime = std::chrono::high_resolution_clock::now();
	if(::Util::endsWith(file, ".parts")) {
		WorldImportExport::loadLoosePartsIntoWorld(file, world);
	} else if(::Util::endsWith(file, ".nativeParts")) {
		WorldImportExport::loadNativePartsIntoWorld(file, world);
	} else if(::Util::endsWith(file, ".world")) {
		WorldImportExport::loadWorld(file, world);
	}
	std::chrono::nanoseconds deltaTime = std::chrono::high_resolution_clock::now() - startTime;
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
		char pathBuffer[MAX_PATH_LENGTH];
		if(saveWorldDialog(pathBuffer)) {
			Log::info("Saving world to: %s", pathBuffer);
			std::unique_lock<UpgradeableMutex>(worldMutex);
			WorldImportExport::saveWorld(pathBuffer, world);
		}
		return true;
	} else if(pressedKey == Keyboard::KEY_O && keyEvent.getModifiers().isCtrlPressed()) {
		handler->setKey(Keyboard::KEY_O, false);
		char pathBuffer[MAX_PATH_LENGTH];
		if(openWorldDialog(pathBuffer)) {
			Log::info("Opening world: %s", pathBuffer);
			std::unique_lock<UpgradeableMutex>(worldMutex);
			WorldImportExport::loadWorld(pathBuffer, world);
		}
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
void togglePause() {
	physicsThread.toggleRunning();
}

void pause() {
	physicsThread.stop();
}

void unpause() {
	physicsThread.start();
}

bool isPaused() {
	return !physicsThread.isRunning();
}

void setSpeed(double newSpeed) {
	physicsThread.speed = newSpeed;
}

double getSpeed() {
	return physicsThread.speed;
}

void runTick() {
	physicsThread.runTick();
}

void toggleFlying() {
	// Through using syncModification, we ensure that the creation or deletion of the player shape is not handled by the physics thread, thus avoiding a race condition with the Registry
	// TODO this is not a proper solution, it should be an asyncModification! But at least it fixes the sporadic crash
	std::unique_lock<UpgradeableMutex> worldLock(worldMutex);
	if (screen.camera.flying) {
		Log::info("Creating player");
		screen.camera.flying = false;
		// this modifies Registry, which may or may not be a race condition
		screen.camera.attachment = new ExtendedPart(polyhedronShape(ShapeLibrary::createPrism(50, 0.3f, 1.5f)), GlobalCFrame(screen.camera.cframe.getPosition()), {1.0, 5.0, 0.0}, "Player");
		screen.world->addPart(screen.camera.attachment);
	} else {
		Log::info("Destroying player");
		delete screen.camera.attachment;
		screen.camera.flying = true;
	}
}
}; // namespace P3D::Application

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