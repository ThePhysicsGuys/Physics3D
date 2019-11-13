#include "core.h"

#include "worldBuilder.h"

#include "application.h"
#include "../physics/math/mathUtil.h"

namespace WorldBuilder {

	Shape dominoShape;
	PartFactory dominoFactory;
	PartFactory legFactory;

	void init() {
		dominoShape = Shape(BoundingBox{-0.1, -0.7, -0.3, 0.1, 0.7, 0.3}.toShape());
		dominoFactory = PartFactory(dominoShape, screen, "domino");
		legFactory = PartFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");
	}

	void createDominoAt(Position pos, Mat3 rotation) {
		ExtendedPart* domino = dominoFactory.produce(GlobalCFrame(pos, rotation), { 1, 0.7, 0.3 });
		world.addPart(domino);
	}

	void makeDominoStrip(int dominoCount, Position dominoStart, Vec3 dominoOffset) {
		for (int i = 0; i < dominoCount; i++) {
			createDominoAt(dominoStart + dominoOffset * i, Mat3());
		}
	}

	void makeDominoTower(int floors, int circumference, Position origin) {
		double radius = circumference / 4.4;
		Mat3 sideways = fromEulerAngles(PI / 2, 0.0, 0.0);
		for (int floor = 0; floor < floors; floor++) {
			for (int j = 0; j < circumference; j++) {
				double angle = (2 * PI * (j + (floor % 2) / 2.0)) / circumference;
				Vec3 pos = Vec3(std::cos(angle) * radius, floor * 0.7 + 0.30, std::sin(angle) * radius);
				createDominoAt(origin + pos, rotY(-angle) * sideways);
			}
		}
	}

	SpiderFactory::SpiderFactory(double spiderSize, int legCount) : spiderSize(spiderSize), legCount(legCount), bodyFactory(Library::createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1), screen, "SpiderBody") {}

	void SpiderFactory::buildSpider(const GlobalCFrame& spiderPosition) {
		//ExtendedPart* spiderBody = createUniquePart(screen, createPointyPrism(legCount, 0.5, 0.2, 0.1, 0.1), spiderPosition, 1.0, 0.0, "SpiderBody");
		ExtendedPart* spiderBody = bodyFactory.produce(spiderPosition, { 1.0, 0.5, 0.3 });
		spiderBody->material.ambient = Vec4f(0.6f, 0.6f, 0.6f, 1.0f);

		//PartFactory legFactory(BoundingBox(0.05, 0.5, 0.05).toShape(), screen, "SpiderLeg");

		CFrame topOfLeg(Vec3(0.0, 0.25, 0.0), fromEulerAngles(0.2, 0.0, 0.0));

		world.addPart(spiderBody);

		Physical* spider = spiderBody->parent;

		for (int i = 0; i < legCount; i++) {
			ExtendedPart* leg = legFactory.produce(GlobalCFrame(), { 1.0, 0.5, 0.3 }, "LegPart " + std::to_string(i));
			leg->material.ambient = Vec4f(0.4f, 0.4f, 0.4f, 1.0f);

			double angle = i * PI * 2 / legCount;

			CFrame attachPointOnBody(rotY(angle) * Vec3(0.5, 0.0, 0.0), rotY(angle + PI / 2));
			CFrame attach = attachPointOnBody.localToGlobal(~topOfLeg);

			spider->attachPart(leg, attach);
		}
	}


	HollowBoxParts buildHollowBox(Bounds box, double wallThickness) {
		Shape YPlateShape = Library::createBox(box.getWidth(), wallThickness, box.getDepth());
		Shape ZPlateShape = Library::createBox(box.getWidth() - wallThickness * 2, box.getHeight() - wallThickness * 2, wallThickness);
		Shape XPlateShape = Library::createBox(wallThickness, box.getHeight() - wallThickness * 2, box.getDepth());

		PartFactory xpf(XPlateShape, screen);
		PartFactory ypf(YPlateShape, screen);
		PartFactory zpf(ZPlateShape, screen);

		ExtendedPart* bottom = ypf.produce(GlobalCFrame(box.getCenter() - Vec3(0, box.getHeight() / 2 - wallThickness / 2, 0)), { 1.0, 0.2, 0.3 }, "BottomPlate");
		ExtendedPart* top = ypf.produce(GlobalCFrame(), { 1.0, 0.2, 0.3 }, "TopPlate");
		ExtendedPart* left = xpf.produce(GlobalCFrame(), { 1.0, 0.2, 0.3 }, "LeftPlate");
		ExtendedPart* right = xpf.produce(GlobalCFrame(), { 1.0, 0.2, 0.3 }, "RightPlate");
		ExtendedPart* front = zpf.produce(GlobalCFrame(), { 1.0, 0.2, 0.3 }, "FrontPlate");
		ExtendedPart* back = zpf.produce(GlobalCFrame(), { 1.0, 0.2, 0.3 }, "BackPlate");

		world.addPart(bottom);
		Physical & parent = *bottom->parent;
		parent.attachPart(top, CFrame(Vec3(0, box.getHeight() - wallThickness, 0)));
		parent.attachPart(left, CFrame(Vec3(box.getWidth() / 2 - wallThickness / 2, box.getHeight() / 2 - wallThickness / 2, 0)));
		parent.attachPart(right, CFrame(Vec3(-box.getWidth() / 2 + wallThickness / 2, box.getHeight() / 2 - wallThickness / 2, 0)));
		parent.attachPart(front, CFrame(Vec3(0, box.getHeight() / 2 - wallThickness / 2, box.getDepth() / 2 - wallThickness / 2)));
		parent.attachPart(back, CFrame(Vec3(0, box.getHeight() / 2 - wallThickness / 2, -box.getDepth() / 2 + wallThickness / 2)));

		return HollowBoxParts{ bottom, top, left, right, front, back };
	}

	void buildPerformanceWorld() {

	}
}