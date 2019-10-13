#pragma once

#include "debug.h"
#include "worlds.h"
#include "../physics/misc/shapeLibrary.h"
#include "extendedPart.h"
#include "partFactory.h"

#include "../physics/geometry/shape.h"
#include "../physics/geometry/convexShapeBuilder.h"

namespace WorldBuilder {
	void init();

	void createDominoAt(Position pos, Mat3 rotation);
	void makeDominoStrip(int dominoCount);
	void makeDominoTower(int floors, int circumference, Vec3 origin);

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
		PartFactory bodyFactory;
		double spiderSize;
		int legCount;

		SpiderFactory(double spiderSize, int legCount);

		void buildSpider(const GlobalCFrame& spiderPosition);
	};

	void buildPerformanceWorld();
}
