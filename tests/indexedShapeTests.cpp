#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"

#include "../physics/geometry/indexedShape.h"
#include "../physics/geometry/shapeBuilder.h"
#include "../physics/geometry/convexShapeBuilder.h"
#include "../physics/misc/shapeLibrary.h"
#include "../physics/misc/validityHelper.h"

TEST_CASE(testIndexedShape) {
	Vec3f verts[]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};

	TriangleNeighbors neighBuf[4];

	fillNeighborBuf(triangles, 4, neighBuf);
	
	IndexedShape s(verts, triangles, 4, 4, neighBuf);

	ASSERT_TRUE(isValid(s));
}

TEST_CASE(buildShape) {
	Vec3f verts[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	ShapeBuilder b(verts, triangles, 4, 4, neighBuf);

	b.addPoint(Vec3f(1, 1, 1), 3);

	IndexedShape result = b.toIndexedShape();

	ASSERT_TRUE(isValid(result));
}

TEST_CASE(buildConvexShape) {
	int builderRemovalBuffer[1000];
	EdgePiece builderAddingBuffer[1000];

	Vec3f verts[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	Polyhedron simpleTetrahedron(verts, triangles, 4, 4);

	ASSERT_TRUE(isValid(simpleTetrahedron));

	ConvexShapeBuilder builder(verts, triangles, 4, 4, neighBuf, builderRemovalBuffer, builderAddingBuffer);

	builder.addPoint(Vec3f(-1, 10, -1), 3);

	ASSERT_TRUE(isValid(builder.toIndexedShape()));

	Vec3f verts2[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles2[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf2[20];

	Polyhedron simpleTetrahedron2(verts2, triangles2, 4, 4);

	ASSERT_TRUE(isValid(simpleTetrahedron2));

	ConvexShapeBuilder builder2(verts2, triangles2, 4, 4, neighBuf2, builderRemovalBuffer, builderAddingBuffer);

	builder2.addPoint(Vec3f(0.4f, 0.4f, 0.4f), 3);

	ASSERT_TRUE(isValid(builder2.toIndexedShape()));

	builder2.addPoint(Vec3f(-0.5f, 50.0f, -0.5f), 3);

	ASSERT_TRUE(isValid(builder2.toIndexedShape()));

	Vec3f newIcosaVerts[30];
	Triangle newIcosaTriangles[40];
	TriangleNeighbors icosaNeighBuf[40];

	ConvexShapeBuilder icosaBuilder(Library::icosahedron, newIcosaVerts, newIcosaTriangles, icosaNeighBuf, builderRemovalBuffer, builderAddingBuffer);

	ASSERT_TRUE(isValid(icosaBuilder.toIndexedShape()));

	icosaBuilder.addPoint(Vec3f(0, 1.1f, 0));
	icosaBuilder.addPoint(Vec3f(0, -1.1f, 0));
	icosaBuilder.addPoint(Vec3f(1.1f, 0, 0));
	icosaBuilder.addPoint(Vec3f(-1.1f, 0, 0));
	icosaBuilder.addPoint(Vec3f(0, 0, 1.1f));
	icosaBuilder.addPoint(Vec3f(0, 0, -1.1f));

	ASSERT_TRUE(isValid(icosaBuilder.toIndexedShape()));
}
