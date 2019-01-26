#include "objectLibrary.h"

#include <cmath>

double g = (1.0 + sqrt(5.0)) / 2.0;

Vec3 icosahedronVertices[] {	
	Vec3(0, 0.5, g/2), Vec3(0, 0.5, -g/2), Vec3(0, -0.5, -g/2), Vec3(0, -0.5, g/2),
	Vec3(0.5, g/2, 0), Vec3(0.5, -g/2, 0), Vec3(-0.5, -g/2, 0), Vec3(-0.5, g/2, 0),
	Vec3(g/2, 0, 0.5), Vec3(-g/2, 0, 0.5), Vec3(-g/2, 0, -0.5), Vec3(g/2, 0, -0.5),
};

// TOP: 0  -  3 8 4 7 9
// BOT: 2  -  10 1 11 5 6

Triangle icosahedronTriangles[] {
	{0 , 3 , 8}, {0 , 8, 4 }, {0, 4 , 7}, {0, 7 , 9}, {0, 9, 3 },
	{2 , 10, 1}, {2 , 1, 11}, {2, 11, 5}, {2, 5 , 6}, {2, 6, 10},
	{8 , 3 , 5}, {8 , 5, 11}, {8, 11, 4}, {4, 11, 1}, {4, 1, 7 },
	{10, 7 , 1}, {10, 9, 7 }, {9, 10, 6}, {9, 6 , 3}, {3, 6, 5 },
};

const NormalizedShape icosahedron(icosahedronVertices, icosahedronTriangles, 12, 20);

// Test shape
Vec3 triangleShapeVecs[] {
	Vec3(0.0, 0.0, 0.0), 
	Vec3(1.0, 0.0, 0.0), 
	Vec3(0.0, 1.0, 0.0), 
	Vec3(0.0, 0.0, 1.0)
};

Triangle triangleShapeTriangles[] {
	{0, 2, 1}, {0, 3, 2}, {0, 1, 3}, {1, 2, 3}
};

const Shape triangleShape(triangleShapeVecs, triangleShapeTriangles, 4, 4);

Vec3 houseVertices[] {
	Vec3(-0.5, 0.0, -0.5), Vec3(-0.5, 0.0, 0.5), Vec3(0.5, 0.0, 0.5), Vec3(0.5, 0.0, -0.5),
	Vec3(-0.5, 1.0, -0.5), Vec3(-0.5, 1.0, 0.5), Vec3(0.5, 1.0, 0.5), Vec3(0.5, 1.0, -0.5),
	Vec3(0.0, 1.5, -0.5), Vec3(0.0, 1.5, 0.5)
};

Triangle houseTriangles[] {
	{0, 1, 4}, {5, 4, 1}, // left
	{0, 2, 1}, {0, 3, 2}, // bottom
	{0, 4, 3}, {3, 4, 7}, // front
	{3, 7, 2}, {2, 7, 6}, // right
	{1, 2, 5}, {5, 2, 6}, // back
	{7, 4, 8}, {5, 6, 9}, // roof
	{4, 5, 9}, {4, 9, 8}, {7, 8, 9}, {7, 9, 6} //roof2
};

const Shape house(houseVertices, houseTriangles, 10, 16);

NormalizedShape createCube(double side) {
	return createBox(side, side, side);
}

NormalizedShape createBox(double width, double height, double length) {
	return BoundingBox{-width / 2, -height / 2, -length / 2, width / 2, height / 2, length / 2}.toShape(new Vec3[8]);
}
