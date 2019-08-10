#pragma once

#include "debug.h"
#include "worlds.h"
#include "shapeLibrary.h"
#include "extendedPart.h"
#include "partFactory.h"

#include "../engine/geometry/shape.h"
#include "../engine/geometry/convexShapeBuilder.h"

namespace WorldBuilder {
	void init();

	void createDominoAt(Vec3 pos, Mat3 rotation);
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

	HollowBoxParts buildHollowBox(BoundingBox box, double wallThickness);

	struct SpiderFactory {
		PartFactory bodyFactory;
		double spiderSize;
		int legCount;

		SpiderFactory(double spiderSize, int legCount);

		void buildSpider(CFrame spiderPosition);
	};

	void buildPerformanceWorld();
}
