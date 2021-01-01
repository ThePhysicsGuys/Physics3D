#pragma once

#include "worlds.h"
#include "extendedPart.h"

#include "../graphics/debug/guiDebug.h"
#include "../physics/geometry/shape.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/geometry/convexShapeBuilder.h"

#include <array>

namespace P3D::Application {

namespace WorldBuilder {
extern Shape wedge;
extern Shape treeTrunk;
extern Shape icosahedron;
extern Shape triangle;
extern Shape arrow;
extern PartProperties basicProperties;

void init();

void createDominoAt(const GlobalCFrame& cframe);
void makeDominoStrip(int dominoCount, Position dominoStart, Vec3 dominoOffset);
void makeDominoTower(int floors, int circumference, Position origin);
void buildTerrain(double width, double depth, int folder = 0);
void buildCar(const GlobalCFrame& location, int folder = 0);
void buildFloor(double width, double depth, int folder = 0);
void buildFloorAndWalls(double width, double depth, double wallHeight, int folder = 0);
void buildConveyor(double width, double length, const GlobalCFrame& cframe, double speed, int folder = 0);

std::array<ExtendedPart*, 3> produceAxes(const GlobalCFrame & cf, const PartProperties& properties, double scale = 1.0);
void attachAxes(ExtendedPart* part, double scale = 1.0);

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

	void buildSpider(const GlobalCFrame& spiderPosition, int folder = 0);
};
}

};