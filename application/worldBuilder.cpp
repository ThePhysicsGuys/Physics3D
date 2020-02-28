#include "core.h"

#include "worldBuilder.h"

#include "application.h"
#include "../physics/math/mathUtil.h"
#include "../physics/geometry/basicShapes.h"

#include "../graphics/gui/gui.h"

#include "../util/resource/resourceLoader.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/resource/textureResource.h"

namespace Application {

namespace WorldBuilder {

Shape wedge;
Shape treeTrunk;
Shape icosahedron;

void init() {
	wedge = Shape(Library::wedge);
	treeTrunk = Shape(Library::createPrism(7, 0.5, 11.0));
	icosahedron = Shape(Library::icosahedron);
}

void createDominoAt(const GlobalCFrame& cframe) {
	ExtendedPart* domino = new ExtendedPart(Box(0.2, 1.4, 0.6), cframe, { 1, 0.7, 0.3 });
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


void buildFloor(double width, double depth) {
	Material floorMaterial = Material(ResourceManager::get<TextureResource>("floorMaterial"));
	ExtendedPart* floorExtendedPart = new ExtendedPart(Box(width, 1.0, depth), GlobalCFrame(0.0, 0.0, 0.0), { 2.0, 1.0, 0.3 });
	floorExtendedPart->material = floorMaterial;
	world.addTerrainPart(floorExtendedPart);
}
void buildFloorAndWalls(double width, double depth, double wallHeight) {
	buildFloor(width, depth);

	PartProperties wallProperties { 2.0, 1.0, 0.3 };
	world.addTerrainPart(new ExtendedPart(Box(0.7, wallHeight, depth), GlobalCFrame(width / 2, wallHeight / 2, 0.0), wallProperties));
	world.addTerrainPart(new ExtendedPart(Box(width, wallHeight, 0.7), GlobalCFrame(0.0, wallHeight / 2, depth / 2), wallProperties));
	world.addTerrainPart(new ExtendedPart(Box(0.7, wallHeight, depth), GlobalCFrame(-width / 2, wallHeight / 2, 0.0), wallProperties));
	world.addTerrainPart(new ExtendedPart(Box(width, wallHeight, 0.7), GlobalCFrame(0.0, wallHeight / 2, -depth / 2), wallProperties));
}

SpiderFactory::SpiderFactory(double spiderSize, int legCount) : spiderSize(spiderSize), legCount(legCount), bodyShape(Library::createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1)) {}

void SpiderFactory::buildSpider(const GlobalCFrame& spiderPosition) {
	//ExtendedPart* spiderBody = createUniquePart(screen, createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1), spiderPosition, 1.0, 0.0, "SpiderBody");
	ExtendedPart* spiderBody = new ExtendedPart(bodyShape, spiderPosition, { 1.0, 0.5, 0.3 });
	spiderBody->material.ambient = Vec4f(0.6f, 0.6f, 0.6f, 1.0f);

	//PartFactory legFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");

	CFrame topOfLeg(Vec3(0.0, 0.25, 0.0), Rotation::fromEulerAngles(0.2, 0.0, 0.0));

	world.addPart(spiderBody);

	Physical* spider = spiderBody->parent;

	for (int i = 0; i < legCount; i++) {
		ExtendedPart* leg = new ExtendedPart(Box(0.05, 0.5, 0.05), GlobalCFrame(), { 1.0, 0.5, 0.3 }, "LegPart " + std::to_string(i));
		leg->material.ambient = Vec4f(0.4f, 0.4f, 0.4f, 1.0f);

		double angle = i * PI * 2 / legCount;

		CFrame attachPointOnBody(Rotation::rotY(angle) * Vec3(0.5, 0.0, 0.0), Rotation::rotY(angle + PI / 2));
		CFrame attach = attachPointOnBody.localToGlobal(~topOfLeg);

		spider->attachPart(leg, attach);
	}
}


HollowBoxParts buildHollowBox(Bounds box, double wallThickness) {
	double width = box.getWidth();
	double height = box.getHeight();
	double depth = box.getDepth();

	Shape XPlate = Box(wallThickness, height - wallThickness * 2, depth);
	Shape YPlate = Box(width, wallThickness, depth);
	Shape ZPlate = Box(width - wallThickness * 2, height - wallThickness * 2, wallThickness);

	ExtendedPart* bottom = new ExtendedPart(YPlate, GlobalCFrame(box.getCenter() - Vec3(0, height / 2 - wallThickness / 2, 0)), { 1.0, 0.2, 0.3 }, "BottomPlate");
	ExtendedPart* top = new ExtendedPart(YPlate, bottom, CFrame(0, height - wallThickness, 0), { 1.0, 0.2, 0.3 }, "TopPlate");
	ExtendedPart* left = new ExtendedPart(XPlate, bottom, CFrame(width / 2 - wallThickness / 2, height / 2 - wallThickness / 2, 0), { 1.0, 0.2, 0.3 }, "LeftPlate");
	ExtendedPart* right = new ExtendedPart(XPlate, bottom, CFrame(-width / 2 + wallThickness / 2, height / 2 - wallThickness / 2, 0), { 1.0, 0.2, 0.3 }, "RightPlate");
	ExtendedPart* front = new ExtendedPart(ZPlate, bottom, CFrame(0, height / 2 - wallThickness / 2, depth / 2 - wallThickness / 2), { 1.0, 0.2, 0.3 }, "FrontPlate");
	ExtendedPart* back = new ExtendedPart(ZPlate, bottom, CFrame(0, height / 2 - wallThickness / 2, -depth / 2 + wallThickness / 2), { 1.0, 0.2, 0.3 }, "BackPlate");

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

void buildTree(Position treePos) {
	GlobalCFrame trunkCFrame(treePos, Rotation::fromEulerAngles(fRand(-0.1, 0.1), fRand(-3.1415, 3.1415), fRand(-0.1, 0.1)));

	ExtendedPart* trunk = new ExtendedPart(treeTrunk, trunkCFrame, { 1.0, 1.0, 0.3 }, "trunk");
	trunk->material.ambient = COLOR::get(0x654321);
	world.addTerrainPart(trunk);

	Position treeTop = trunkCFrame.localToGlobal(Vec3(0.0, 5.5, 0.0));


	for (int j = 0; j < 15; j++) {
		GlobalCFrame leavesCFrame(treeTop + Vec3(fRand(-1.0, 1.0), fRand(-1.0, 1.0), fRand(-1.0, 1.0)), Rotation::fromEulerAngles(fRand(0.0, 3.1415), fRand(0.0, 3.1415), fRand(0.0, 3.1415)));
		ExtendedPart* leaves = new ExtendedPart(icosahedron.scaled(2.1, 1.9, 1.7), leavesCFrame, { 1.0, 1.0, 0.3 }, "trunk");

		leaves->material.ambient = Vec4(fRand(-0.2, 0.2), 0.6 + fRand(-0.2, 0.2), 0.0, 1.0);

		world.addTerrainPart(leaves);
	}
}

void buildConveyor(double width, double length, const GlobalCFrame& cframe, double speed) {
	ExtendedPart* conveyor = new ExtendedPart(Box(width, 0.3, length), cframe, { 1.0, 0.8, 0.0, Vec3(0.0, 0.0, speed) }, "Conveyor");
	conveyor->material.ambient = Vec4f(0.2, 0.2, 0.2, 1.0);
	world.addTerrainPart(conveyor);
	ExtendedPart* leftWall = new ExtendedPart(Box(0.2, 0.6, length), cframe.localToGlobal(CFrame(-width / 2 - 0.1, 0.1, 0.0)), { 1.0, 0.4, 0.3, Vec3(0.0, 0.0, 0.0) }, "Wall");
	world.addTerrainPart(leftWall);
	ExtendedPart* rightWall = new ExtendedPart(Box(0.2, 0.6, length), cframe.localToGlobal(CFrame(width / 2 + 0.1, 0.1, 0.0)), { 1.0, 0.4, 0.3, Vec3(0.0, 0.0, 0.0) }, "Wall");
	world.addTerrainPart(rightWall);
}

void buildTerrain(double width, double depth) {
	Log::subject s("Terrain");
	Log::info("Starting terrain building!");

	int maxProgress = 10;

	int lastProgress = 0;

	Log::info("0%%");
	for (double x = -width / 2; x < width / 2; x += 3.0) {
		for (double z = -depth / 2; z < depth / 2; z += 3.0) {
			double yOffset = getYOffset(x, z);
			Position pos((x / 3.0 + fRand(0.0, 1.0)) * 3.0, fRand(0.0, 1.0) + yOffset, (z / 3.0 + fRand(0.0, 1.0)) * 3.0);
			GlobalCFrame cf(pos, Rotation::fromEulerAngles(fRand(0.0, 3.1415), fRand(0.0, 3.1415), fRand(0.0, 3.1415)));
			ExtendedPart* newPart = new ExtendedPart(icosahedron.scaled(4.0, 4.0, 4.0), cf, { 1.0, 1.0, 0.3 }, "terrain");
			newPart->material.ambient = Vec4(0.0, yOffset / 40.0 + 0.5, 0.0, 1.0);
			world.addTerrainPart(newPart);
		}

		double progress = (x + width / 2) / width;

		int progressToInt = progress * maxProgress;

		if (progressToInt > lastProgress) {
			Log::info("%d%%", int(progressToInt * 100.0 / maxProgress));
		}
		lastProgress = progressToInt;
	}
	Log::info("100%%");


	Log::info("Finished terrain, adding trees!");
	for (int i = 0; i < width * depth / 70; i++) {
		double x = fRand(-width / 2, width / 2);
		double z = fRand(-depth / 2, depth / 2);

		if (abs(x) < 30.0 && abs(z) < 30.0) continue;

		Position treePos(x, getYOffset(x, z) + 8.0, z);

		buildTree(treePos);
	}
	Log::info("Optimizing terrain! (This will take around 1.5x the time it took to build the world)");
	world.optimizeTerrain();
}
void buildCar(const GlobalCFrame& location) {
	PartProperties carProperties { 1.0, 0.7, 0.3 };
	PartProperties wheelProperties { 1.0, 2.0, 0.7 };

	ExtendedPart* carBody = new ExtendedPart(Box(2.0, 0.1, 1.0), location, carProperties, "CarBody");
	ExtendedPart* carLeftPanel = new ExtendedPart(Box(2.0, 0.4, 0.1), carBody, CFrame(0.0, 0.25, -0.5), carProperties, "CarLeftSide");
	ExtendedPart* carRightPanel = new ExtendedPart(Box(2.0, 0.4, 0.1), carBody, CFrame(0.0, 0.25, 0.5), carProperties, "CarRightSide");
	ExtendedPart* carLeftWindow = new ExtendedPart(Box(1.4, 0.8, 0.05), carLeftPanel, CFrame(-0.3, 0.6, 0.0), carProperties, "WindowLeft");
	ExtendedPart* carWedgeLeft = new ExtendedPart(wedge.scaled(0.6, 0.8, 0.1), carLeftWindow, CFrame(1.0, 0.0, 0.0), carProperties, "WedgeLeft");
	ExtendedPart* carRightWindow = new ExtendedPart(Box(1.4, 0.8, 0.05), carRightPanel, CFrame(-0.3, 0.6, 0.0), carProperties, "WindowRight");
	ExtendedPart* carWedgeRight = new ExtendedPart(wedge.scaled(0.6, 0.8, 0.1), carRightWindow, CFrame(1.0, 0.0, 0.0), carProperties, "WedgeRight");
	ExtendedPart* carFrontPanel = new ExtendedPart(Box(0.1, 0.4, 1.0), carBody, CFrame(1.0, 0.25, 0.0), carProperties, "FrontPanel");
	ExtendedPart* carTrunkPanel = new ExtendedPart(Box(0.1, 1.2, 1.0), carBody, CFrame(-1.0, 0.65, 0.0), carProperties, "TrunkPanel");
	ExtendedPart* carRoof = new ExtendedPart(Box(1.4, 0.1, 1.0), carBody, CFrame(-0.3, 1.25, 0.0), carProperties, "Roof");

	ExtendedPart* carWindshield = new ExtendedPart(Box(1.0, 0.05, 1.0), carBody, CFrame(Vec3(0.7, 0.85, 0.0), Rotation::fromEulerAngles(0.0, 0.0, -0.91)), carProperties, "Windshield");
	ExtendedPart* wheel1 = new ExtendedPart(Sphere(0.25), location.localToGlobal(CFrame(0.8, 0.0, 0.8)), wheelProperties, "Wheel");
	ExtendedPart* wheel2 = new ExtendedPart(Sphere(0.25), location.localToGlobal(CFrame(0.8, 0.0, -0.8)), wheelProperties, "Wheel");
	ExtendedPart* wheel3 = new ExtendedPart(Sphere(0.25), location.localToGlobal(CFrame(-0.8, 0.0, 0.8)), wheelProperties, "Wheel");
	ExtendedPart* wheel4 = new ExtendedPart(Sphere(0.25), location.localToGlobal(CFrame(-0.8, 0.0, -0.8)), wheelProperties, "Wheel");

	carLeftWindow->material.ambient = Vec4f(0.7, 0.7, 1.0, 0.5);
	carRightWindow->material.ambient = Vec4f(0.7, 0.7, 1.0, 0.5);
	carWindshield->material.ambient = Vec4f(0.7, 0.7, 1.0, 0.5);

	world.addPart(carBody);

	world.addPart(wheel1);
	world.addPart(wheel2);
	world.addPart(wheel3);
	world.addPart(wheel4);

	ConstraintGroup car;
	car.ballConstraints.push_back(BallConstraint { Vec3(0.8, 0.0, 0.8), carBody->parent, Vec3(0,0,0), wheel1->parent });
	car.ballConstraints.push_back(BallConstraint { Vec3(0.8, 0.0, -0.8), carBody->parent, Vec3(0,0,0), wheel2->parent });
	car.ballConstraints.push_back(BallConstraint { Vec3(-0.8, 0.0, 0.8), carBody->parent, Vec3(0,0,0), wheel3->parent });
	car.ballConstraints.push_back(BallConstraint { Vec3(-0.8, 0.0, -0.8), carBody->parent, Vec3(0,0,0), wheel4->parent });
	world.constraints.push_back(std::move(car));
}
}

};