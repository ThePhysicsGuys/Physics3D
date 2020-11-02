#include "core.h"

#include "worldBuilder.h"

#include "application.h"
#include "../physics/math/mathUtil.h"
#include "../physics/geometry/shape.h"
#include "../physics/geometry/shapeCreation.h"

#include "../graphics/gui/gui.h"

#include "../util/resource/resourceLoader.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/resource/textureResource.h"
#include "../physics/math/constants.h"
#include "view/screen.h"
#include "ecs/components.h"

namespace P3D::Application {

namespace WorldBuilder {

Shape wedge;
Shape treeTrunk;
Shape icosahedron;

void init() {
	wedge = polyhedronShape(Library::wedge);
	treeTrunk = polyhedronShape(Library::createPrism(7, 0.5, 11.0).rotated(Rotation::Predefined::X_270));
	icosahedron = polyhedronShape(Library::icosahedron);
}

void createDominoAt(const GlobalCFrame& cframe) {
	ExtendedPart* domino = new ExtendedPart(boxShape(0.2, 1.4, 0.6), cframe, { 1, 0.7, 0.3 });
	world.addPart(domino);
}

void makeDominoStrip(int dominoCount, Position dominoStart, Vec3 dominoOffset) {
	for (int i = 0; i < dominoCount; i++) {
		createDominoAt(GlobalCFrame(dominoStart + dominoOffset * i));
	}
}

void makeDominoTower(int floors, int circumference, Position origin) {
	double radius = circumference / 4.4;
	Rotation sideways = Rotation::fromEulerAngles(PI / 2, 0.0, 0.0);
	for (int floor = 0; floor < floors; floor++) {
		for (int j = 0; j < circumference; j++) {
			double angle = (2 * PI * (j + (floor % 2) / 2.0)) / circumference;
			Vec3 pos = Vec3(std::cos(angle) * radius, floor * 0.7 + 0.30, std::sin(angle) * radius);
			createDominoAt(GlobalCFrame(origin + pos, Rotation::rotY(-angle) * sideways));
		}
	}
}


void buildFloor(double width, double depth, int folder) {
	ExtendedPart* floorExtendedPart = new ExtendedPart(boxShape(width, 1.0, depth), GlobalCFrame(0.0, 0.0, 0.0), { 2.0, 1.0, 0.3 }, "Floor", folder);
	screen.registry.getOrAdd<Comp::Material>(floorExtendedPart->entity)->set(Comp::Material::ALBEDO, ResourceManager::get<Graphics::TextureResource>("floorMaterial"));
	world.addTerrainPart(floorExtendedPart);
}
void buildFloorAndWalls(double width, double depth, double wallHeight, int folder) {
	auto floorWallFolder = screen.registry.create(folder);
	screen.registry.add<Comp::Name>(floorWallFolder, "Floor and walls");
	
	buildFloor(width, depth, floorWallFolder);

	PartProperties wallProperties { 2.0, 1.0, 0.3 };
	world.addTerrainPart(new ExtendedPart(boxShape(0.7, wallHeight, depth), GlobalCFrame(width / 2, wallHeight / 2, 0.0), wallProperties, "Wall", floorWallFolder));
	world.addTerrainPart(new ExtendedPart(boxShape(width, wallHeight, 0.7), GlobalCFrame(0.0, wallHeight / 2, depth / 2), wallProperties, "Wall", floorWallFolder));
	world.addTerrainPart(new ExtendedPart(boxShape(0.7, wallHeight, depth), GlobalCFrame(-width / 2, wallHeight / 2, 0.0), wallProperties, "Wall", floorWallFolder));
	world.addTerrainPart(new ExtendedPart(boxShape(width, wallHeight, 0.7), GlobalCFrame(0.0, wallHeight / 2, -depth / 2), wallProperties, "Wall", floorWallFolder));
}

SpiderFactory::SpiderFactory(double spiderSize, int legCount) : spiderSize(spiderSize), legCount(legCount), bodyShape(polyhedronShape(Library::createPointyPrism(legCount, 0.5f, 0.2f, 0.1f, 0.1f))) {}

void SpiderFactory::buildSpider(const GlobalCFrame& spiderPosition, int folder) {
	//ExtendedPart* spiderBody = createUniquePart(screen, createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1), spiderPosition, 1.0, 0.0, "SpiderBody");

	auto spiderID = screen.registry.create();
	screen.registry.add<Comp::Name>(spiderID, "Spider");

	if (folder != 0)
		screen.registry.setParent(spiderID, folder);
	
	ExtendedPart* spiderBody = new ExtendedPart(bodyShape, spiderPosition, { 1.0, 0.5, 0.3 }, "Body", spiderID);
	screen.registry.getOrAdd<Comp::Material>(spiderBody->entity)->albedo = Color(0.6f, 0.6f, 0.6f, 1.0f);

	//PartFactory legFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");

	CFrame topOfLeg(Vec3(0.0, 0.25, 0.0), Rotation::fromEulerAngles(0.2, 0.0, 0.0));

	world.addPart(spiderBody);

	Physical* spider = spiderBody->parent;

	for (int i = 0; i < legCount; i++) {
		ExtendedPart* leg = new ExtendedPart(boxShape(0.05, 0.5, 0.05), GlobalCFrame(), { 1.0, 0.5, 0.3 }, "LegPart " + std::to_string(i), spiderID);
		screen.registry.getOrAdd<Comp::Material>(leg->entity)->albedo = Color(0.4f, 0.4f, 0.4f, 1.0f);

		double angle = i * PI * 2 / legCount;

		CFrame attachPointOnBody(Rotation::rotY(angle) * Vec3(0.5, 0.0, 0.0), Rotation::rotY(angle + PI / 2));
		CFrame attach = attachPointOnBody.localToGlobal(~topOfLeg);

		spider->attachPart(leg, attach);
	}
}


HollowBoxParts buildHollowBox(Bounds box, double wallThickness) {
	double width = static_cast<double>(box.getWidth());
	double height = static_cast<double>(box.getHeight());
	double depth = static_cast<double>(box.getDepth());

	Shape XPlate = boxShape(wallThickness, height - wallThickness * 2, depth);
	Shape YPlate = boxShape(width, wallThickness, depth);
	Shape ZPlate = boxShape(width - wallThickness * 2, height - wallThickness * 2, wallThickness);

	auto hollowBoxFolder = screen.registry.create();
	screen.registry.add<Comp::Name>(hollowBoxFolder, "Hollow Box");
	
	ExtendedPart* bottom = new ExtendedPart(YPlate, GlobalCFrame(box.getCenter() - Vec3(0, height / 2 - wallThickness / 2, 0)), { 1.0, 0.2, 0.3 }, "BottomPlate", hollowBoxFolder);
	ExtendedPart* top = new ExtendedPart(YPlate, bottom, CFrame(0, height - wallThickness, 0), { 1.0, 0.2, 0.3 }, "TopPlate");
	ExtendedPart* left = new ExtendedPart(XPlate, bottom, CFrame(width / 2 - wallThickness / 2, height / 2 - wallThickness / 2, 0), { 1.0, 0.2, 0.3 }, "LeftPlate", hollowBoxFolder);
	ExtendedPart* right = new ExtendedPart(XPlate, bottom, CFrame(-width / 2 + wallThickness / 2, height / 2 - wallThickness / 2, 0), { 1.0, 0.2, 0.3 }, "RightPlate", hollowBoxFolder);
	ExtendedPart* front = new ExtendedPart(ZPlate, bottom, CFrame(0, height / 2 - wallThickness / 2, depth / 2 - wallThickness / 2), { 1.0, 0.2, 0.3 }, "FrontPlate", hollowBoxFolder);
	ExtendedPart* back = new ExtendedPart(ZPlate, bottom, CFrame(0, height / 2 - wallThickness / 2, -depth / 2 + wallThickness / 2), { 1.0, 0.2, 0.3 }, "BackPlate", hollowBoxFolder);

	return HollowBoxParts { bottom, top, left, right, front, back };
}

double getYOffset(double x, double z) {
	double regimeYOffset = -10 * cos(x / 30.0 + z / 20.0) - 7 * sin(-x / 25.0 + z / 17.0 + 2.4) + 2 * sin(x / 4.0 + z / 7.0) + sin(x / 9.0 - z / 3.0) + sin(-x / 3.0 + z / 5.0);
	double distFromOriginSq = x * x + z * z;
	if (distFromOriginSq < 5000.0) {
		double factor = distFromOriginSq / 5000.0;
		return regimeYOffset * factor + (1 - factor) * -5;
	} else {
		return regimeYOffset;
	}
}

void buildTree(Position treePos, int treeFolder) {
	GlobalCFrame trunkCFrame(treePos, Rotation::fromEulerAngles(fRand(-0.1, 0.1), fRand(-3.1415, 3.1415), fRand(-0.1, 0.1)));

	ExtendedPart* trunk = new ExtendedPart(treeTrunk, trunkCFrame, { 1.0, 1.0, 0.3 }, "Trunk", treeFolder);
	screen.registry.getOrAdd<Comp::Material>(trunk->entity)->albedo = Graphics::COLOR::get(0x654321);
	world.addTerrainPart(trunk);

	Position treeTop = trunkCFrame.localToGlobal(Vec3(0.0, 5.5, 0.0));

	auto leavesFolder = screen.registry.create(treeFolder);
	screen.registry.add<Comp::Name>(leavesFolder, "Leaves");

	for (int j = 0; j < 15; j++) {
		GlobalCFrame leavesCFrame(treeTop + Vec3(fRand(-1.0, 1.0), fRand(-1.0, 1.0), fRand(-1.0, 1.0)), Rotation::fromEulerAngles(fRand(0.0, 3.1415), fRand(0.0, 3.1415), fRand(0.0, 3.1415)));
		ExtendedPart* leaves = new ExtendedPart(icosahedron.scaled(2.1, 1.9, 1.7), leavesCFrame, { 1.0, 1.0, 0.3 }, "Leaf", leavesFolder);

		screen.registry.getOrAdd<Comp::Material>(leaves->entity)->albedo = Vec4(fRand(-0.2, 0.2), 0.6 + fRand(-0.2, 0.2), 0.0, 1.0);

		world.addTerrainPart(leaves);
	}
}

void buildConveyor(double width, double length, const GlobalCFrame& cframe, double speed, int folder) {
	auto conveyorFolder = screen.registry.create(folder);
	screen.registry.add<Comp::Name>(conveyorFolder, "Conveyor");
	
	ExtendedPart* conveyor = new ExtendedPart(boxShape(width, 0.3, length), cframe, { 1.0, 0.8, 0.0, Vec3(0.0, 0.0, speed) }, "Conveyor", conveyorFolder);
	screen.registry.getOrAdd<Comp::Material>(conveyor->entity)->albedo = Color(0.2f, 0.2f, 0.2f, 1.0f);
	world.addTerrainPart(conveyor);
	ExtendedPart* leftWall = new ExtendedPart(boxShape(0.2, 0.6, length), cframe.localToGlobal(CFrame(-width / 2 - 0.1, 0.1, 0.0)), { 1.0, 0.4, 0.3, Vec3(0.0, 0.0, 0.0) }, "Left Wall", conveyorFolder);
	world.addTerrainPart(leftWall);
	ExtendedPart* rightWall = new ExtendedPart(boxShape(0.2, 0.6, length), cframe.localToGlobal(CFrame(width / 2 + 0.1, 0.1, 0.0)), { 1.0, 0.4, 0.3, Vec3(0.0, 0.0, 0.0) }, "Right Wall", conveyorFolder);
	world.addTerrainPart(rightWall);
}

void buildTerrain(double width, double depth, int folder) {
	Log::subject s("Terrain");
	Log::info("Starting terrain building!");

	int maxProgress = 10;
	int lastProgress = 0;

	auto groundFolder = screen.registry.create(folder);
	screen.registry.add<Comp::Name>(groundFolder, "Ground");
	
	Log::info("0%%");
	for (double x = -width / 2; x < width / 2; x += 3.0) {
		for (double z = -depth / 2; z < depth / 2; z += 3.0) {
			double yOffset = getYOffset(x, z);
			Position pos((x / 3.0 + fRand(0.0, 1.0)) * 3.0, fRand(0.0, 1.0) + yOffset, (z / 3.0 + fRand(0.0, 1.0)) * 3.0);
			GlobalCFrame cf(pos, Rotation::fromEulerAngles(fRand(0.0, 3.1415), fRand(0.0, 3.1415), fRand(0.0, 3.1415)));
			ExtendedPart* newPart = new ExtendedPart(icosahedron.scaled(4.0, 4.0, 4.0), cf, { 1.0, 1.0, 0.3 }, "Ground", groundFolder);
			screen.registry.getOrAdd<Comp::Material>(newPart->entity)->albedo = Color(0.0, yOffset / 40.0 + 0.5, 0.0, 1.0);
			world.addTerrainPart(newPart);
		}

		double progress = (x + width / 2) / width;
		int progressToInt = static_cast<int>(progress * maxProgress);

		if (progressToInt > lastProgress) 
			Log::info("%d%%", int(progressToInt * 100.0 / maxProgress));
		
		lastProgress = progressToInt;
	}
	Log::info("100%%");

	auto treeFolder = screen.registry.create(folder);
	screen.registry.add<Comp::Name>(treeFolder, "Tree");
	
	Log::info("Finished terrain, adding trees!");
	for (int i = 0; i < width * depth / 70; i++) {
		double x = fRand(-width / 2, width / 2);
		double z = fRand(-depth / 2, depth / 2);

		if (std::abs(x) < 30.0 && std::abs(z) < 30.0) continue;

		Position treePos(x, getYOffset(x, z) + 8.0, z);

		buildTree(treePos, treeFolder);
	}
	
	Log::info("Optimizing terrain! (This will take around 1.5x the time it took to build the world)");
	world.optimizeLayers();
}
void buildCar(const GlobalCFrame& location, int folder) {
	PartProperties carProperties { 1.0, 0.7, 0.3 };
	PartProperties wheelProperties { 1.0, 2.0, 0.7 };

	auto carFolder = screen.registry.create(folder);
	screen.registry.add<Comp::Name>(carFolder, "Car");

	ExtendedPart* carBody = new ExtendedPart(boxShape(2.0, 0.1, 1.0), location, carProperties, "CarBody", carFolder);
	ExtendedPart* carLeftPanel = new ExtendedPart(boxShape(2.0, 0.4, 0.1), carBody, CFrame(0.0, 0.25, -0.5), carProperties, "CarLeftSide", carFolder);
	ExtendedPart* carRightPanel = new ExtendedPart(boxShape(2.0, 0.4, 0.1), carBody, CFrame(0.0, 0.25, 0.5), carProperties, "CarRightSide", carFolder);
	ExtendedPart* carLeftWindow = new ExtendedPart(boxShape(1.4, 0.8, 0.05), carLeftPanel, CFrame(-0.3, 0.6, 0.0), carProperties, "WindowLeft", carFolder);
	ExtendedPart* carWedgeLeft = new ExtendedPart(wedge.scaled(0.6, 0.8, 0.1), carLeftWindow, CFrame(1.0, 0.0, 0.0), carProperties, "WedgeLeft", carFolder);
	ExtendedPart* carRightWindow = new ExtendedPart(boxShape(1.4, 0.8, 0.05), carRightPanel, CFrame(-0.3, 0.6, 0.0), carProperties, "WindowRight", carFolder);
	ExtendedPart* carWedgeRight = new ExtendedPart(wedge.scaled(0.6, 0.8, 0.1), carRightWindow, CFrame(1.0, 0.0, 0.0), carProperties, "WedgeRight", carFolder);
	ExtendedPart* carFrontPanel = new ExtendedPart(boxShape(0.1, 0.4, 1.0), carBody, CFrame(1.0, 0.25, 0.0), carProperties, "FrontPanel", carFolder);
	ExtendedPart* carTrunkPanel = new ExtendedPart(boxShape(0.1, 1.2, 1.0), carBody, CFrame(-1.0, 0.65, 0.0), carProperties, "TrunkPanel", carFolder);
	ExtendedPart* carRoof = new ExtendedPart(boxShape(1.4, 0.1, 1.0), carBody, CFrame(-0.3, 1.25, 0.0), carProperties, "Roof", carFolder);

	ExtendedPart* carWindshield = new ExtendedPart(boxShape(1.0, 0.05, 1.0), carBody, CFrame(Vec3(0.7, 0.85, 0.0), Rotation::fromEulerAngles(0.0, 0.0, -0.91)), carProperties, "Windshield", carFolder);
	ExtendedPart* wheel1 = new ExtendedPart(sphereShape(0.25), location.localToGlobal(CFrame(0.8, 0.0, 0.8)), wheelProperties, "Wheel", carFolder);
	ExtendedPart* wheel2 = new ExtendedPart(sphereShape(0.25), location.localToGlobal(CFrame(0.8, 0.0, -0.8)), wheelProperties, "Wheel", carFolder);
	ExtendedPart* wheel3 = new ExtendedPart(sphereShape(0.25), location.localToGlobal(CFrame(-0.8, 0.0, 0.8)), wheelProperties, "Wheel", carFolder);
	ExtendedPart* wheel4 = new ExtendedPart(sphereShape(0.25), location.localToGlobal(CFrame(-0.8, 0.0, -0.8)), wheelProperties, "Wheel", carFolder);

	screen.registry.getOrAdd<Comp::Material>(carLeftWindow->entity)->albedo = Color(0.7f, 0.7f, 1.0f, 0.5f);
	screen.registry.getOrAdd<Comp::Material>(carRightWindow->entity)->albedo = Color(0.7f, 0.7f, 1.0f, 0.5f);
	screen.registry.getOrAdd<Comp::Material>(carWindshield->entity)->albedo = Color(0.7f, 0.7f, 1.0f, 0.5f);

	world.addPart(carBody);

	world.addPart(wheel1);
	world.addPart(wheel2);
	world.addPart(wheel3);
	world.addPart(wheel4);

	ConstraintGroup car;
	car.add(carBody->parent, wheel1->parent, new BallConstraint(Vec3(0.8, 0.0, 0.8), Vec3(0,0,0)));
	car.add(carBody->parent, wheel2->parent, new BallConstraint(Vec3(0.8, 0.0, -0.8), Vec3(0,0,0)));
	car.add(carBody->parent, wheel3->parent, new BallConstraint(Vec3(-0.8, 0.0, 0.8), Vec3(0,0,0)));
	car.add(carBody->parent, wheel4->parent, new BallConstraint(Vec3(-0.8, 0.0, -0.8), Vec3(0,0,0)));
	world.constraints.push_back(std::move(car));
}
}

};