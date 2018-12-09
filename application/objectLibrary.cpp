#include "objectLibrary.h"

#include <cmath>

double g = (1.0 + sqrt(5.0)) / 2.0;

Vec3 icosaVerts[]{	Vec3(0, 0.5, g/2), Vec3(0, 0.5, -g/2), Vec3(0, -0.5, -g/2), Vec3(0, -0.5, g/2),
					Vec3(0.5, g/2, 0), Vec3(0.5, -g/2, 0), Vec3(-0.5, -g/2, 0), Vec3(-0.5, g/2, 0),
					Vec3(g/2, 0, 0.5), Vec3(-g/2, 0, 0.5), Vec3(-g/2, 0, -0.5), Vec3(g/2, 0, -0.5),
};

// TOP: 0  -  3 8 4 7 9
// BOT: 2  -  10 1 11 5 6

Triangle icosaTriangles[]{
	{0, 3, 8},{0, 8, 4},{0, 4, 7},{0, 7, 9},{0, 9, 3},
	{2,10, 1},{2, 1,11},{2,11, 5},{2, 5, 6},{2, 6,10},
	{8, 3, 5},{8, 5, 11},{8, 11, 4},{4, 11, 1},{4, 1, 7},
	{10, 7, 1},{10, 9, 7},{9, 10, 6},{9, 6, 3},{3, 6, 5},
};

Shape icosahedron(icosaVerts, icosaTriangles, 12, 20);


// Test shape
Vec3 triangleShapeVecs[]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 1.0, 0.0), Vec3(0.0, 0.0, 1.0)};
Triangle triangleShapeTriangles[]{{0,2,1},{0,3,2},{0,1,3},{1,2,3}};
Shape triangleShape(triangleShapeVecs, triangleShapeTriangles, 4, 4);