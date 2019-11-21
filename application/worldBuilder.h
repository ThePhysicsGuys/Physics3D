#pragma once

#include "worlds.h"
#include "extendedPart.h"

#include "../graphics/debug/debug.h"
#include "../physics/geometry/shape.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/geometry/convexShapeBuilder.h"

namespace WorldBuilder {
	void init();

	void createDominoAt(Position pos, Mat3 rotation);
	void makeDominoStrip(int dominoCount, Position dominoStart, Vec3 dominoOffset);
	void makeDominoTower(int floors, int circumference, Position origin);
	void buildTerrain(double width, double depth);
	void buildCar(const GlobalCFrame& location);
	void buildFloor(double width, double depth);
	void buildFloorAndWalls(double width, double depth, double wallHeight);
	void buildConveyor(double width, double length, const GlobalCFrame& cframe, double speed);

	struct HollowBoxParts {
		ExtendedPart* bottom;
		ExtendedPart* top;
		ExtendedPart* left;
		ExtendedPart* right;
		ExtendedPart* front;
		ExtendedPart* back;
	};

	HollowBoxParts buildHollowBox(Bounds box, double wallThickness);

	struct SpiderFactory {
		Shape bodyShape;
		double spiderSize;
		int legCount;

		SpiderFactory(double spiderSize, int legCount);

		void buildSpider(const GlobalCFrame& spiderPosition);
	};
}
