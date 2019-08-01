#include "worldBuilder.h"

#include "application.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

Shape dominoShape = BoundingBox{ -0.1, -0.7, -0.3, 0.1, 0.7, 0.3 }.toShape();
PartFactory dominoFactory;
PartFactory legFactory;


void createDominoAt(Vec3 pos, Mat3 rotation) {
	ExtendedPart* domino = dominoFactory.produce(CFrame(pos, rotation), 1, 0.1);
	world.addPart(domino);
}

void makeDominoStrip(int dominoCount) {
	for (int i = 0; i < dominoCount; i++) {
		createDominoAt(Vec3(i * 0.5, 0.7, 1.3), Mat3());
	}
}

void makeDominoTower(int floors, int circumference, Vec3 origin) {
	double radius = circumference / 4.4;
	Mat3 sideways = fromEulerAngles(M_PI / 2, 0.0, 0.0);
	for (int floor = 0; floor < floors; floor++) {
		for (int j = 0; j < circumference; j++) {
			double angle = (2 * M_PI * (j + (floor % 2) / 2.0)) / circumference;
			Vec3 pos = Vec3(std::cos(angle) * radius, floor * 0.7 + 0.30, std::sin(angle) * radius);
			createDominoAt(pos + origin, rotY(-angle) * sideways);
		}
	}
}

SpiderFactory::SpiderFactory(double spiderSize, int legCount) : spiderSize(spiderSize), legCount(legCount), bodyFactory(createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1), screen, "SpiderBody") {}

void SpiderFactory::buildSpider(CFrame spiderPosition) {
	//ExtendedPart* spiderBody = createUniquePart(screen, createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1), spiderPosition, 1.0, 0.0, "SpiderBody");
	ExtendedPart* spiderBody = bodyFactory.produce(spiderPosition, 1.0, 0.5);
	spiderBody->material.ambient = Vec4f(0.6f, 0.6f, 0.6f, 1.0f);

	//PartFactory legFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");

	CFrame topOfLeg(Vec3(0.0, 0.25, 0.0), fromEulerAngles(0.2, 0.0, 0.0));

	world.addPart(spiderBody);

	Physical* spider = spiderBody->parent;

	for (int i = 0; i < legCount; i++) {
		ExtendedPart* leg = legFactory.produce(CFrame(), 1.0, 0.5, std::string("LegPart ") + std::to_string(i));
		leg->material.ambient = Vec4f(0.4f, 0.4f, 0.4f, 1.0f);

		double angle = i * M_PI * 2 / legCount;

		CFrame attachPointOnBody(rotY(angle) * Vec3(0.5, 0.0, 0.0), rotY(angle + M_PI / 2));
		CFrame attach = attachPointOnBody.localToGlobal(~topOfLeg);

		spider->attachPart(leg, attach);
	}
}


HollowBoxParts buildHollowBox(BoundingBox box, double wallThickness) {
	Shape YPlateShape = createBox(box.getWidth(), wallThickness, box.getDepth());
	Shape ZPlateShape = createBox(box.getWidth() - wallThickness * 2, box.getHeight() - wallThickness * 2, wallThickness);
	Shape XPlateShape = createBox(wallThickness, box.getHeight() - wallThickness * 2, box.getDepth());

	PartFactory xpf(XPlateShape, screen);
	PartFactory ypf(YPlateShape, screen);
	PartFactory zpf(ZPlateShape, screen);

	ExtendedPart* bottom = ypf.produce(CFrame(Vec3(box.getCenter() - Vec3(0, box.getHeight() / 2 - wallThickness / 2, 0))), 1.0, 0.2, "BottomPlate");
	ExtendedPart * top = ypf.produce(CFrame(), 1.0, 0.2, "TopPlate");
	ExtendedPart * left = xpf.produce(CFrame(), 1.0, 0.2, "LeftPlate");
	ExtendedPart * right = xpf.produce(CFrame(), 1.0, 0.2, "RightPlate");
	ExtendedPart * front = zpf.produce(CFrame(), 1.0, 0.2, "FrontPlate");
	ExtendedPart * back = zpf.produce(CFrame(), 1.0, 0.2, "BackPlate");

	world.addPart(bottom);
	Physical & parent = *bottom->parent;
	world.attachPart(top, parent, CFrame(Vec3(0, box.getHeight() - wallThickness, 0)));
	world.attachPart(left, parent, CFrame(Vec3(box.getWidth() / 2 - wallThickness / 2, box.getHeight() / 2 - wallThickness / 2, 0)));
	world.attachPart(right, parent, CFrame(Vec3(-box.getWidth() / 2 + wallThickness / 2, box.getHeight() / 2 - wallThickness / 2, 0)));
	world.attachPart(front, parent, CFrame(Vec3(0, box.getHeight() / 2 - wallThickness / 2, box.getDepth() / 2 - wallThickness / 2)));
	world.attachPart(back, parent, CFrame(Vec3(0, box.getHeight() / 2 - wallThickness / 2, -box.getDepth() / 2 + wallThickness / 2)));

	return HollowBoxParts{ bottom, top, left, right, front, back };
}

void buildPerformanceWorld() {

}