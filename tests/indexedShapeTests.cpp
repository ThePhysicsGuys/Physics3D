#include "testsMain.h"
#include "../physics/geometry/indexedShape.h"
#include "../physics/geometry/shapeBuilder.h"
#include "../physics/geometry/convexShapeBuilder.h"
#include "../physics/misc/shapeLibrary.h"

TEST_CASE(testIndexedShape) {
	Vec3f verts[]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};

	TriangleNeighbors neighBuf[4];

	fillNeighborBuf(triangles, 4, neighBuf);
	
	auto triPtr = SharedArrayPtr<const Triangle>::staticSharedArrayPtr(triangles);

	IndexedShape s(verts, triPtr, 4, 4, neighBuf);

	ASSERT_TRUE(s.isValid());
}

TEST_CASE(buildShape) {
	Vec3f verts[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[10]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	ShapeBuilder b(verts, triangles, 4, 4, neighBuf);

	b.addPoint(Vec3f(1, 1, 1), 3);

	ASSERT_TRUE(b.toIndexedShape().isValid());
}

TEST_CASE(buildConvexShape) {
	int builderRemovalBuffer[1000];
	EdgePiece builderAddingBuffer[1000];

	Vec3f verts[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	auto triPtr = SharedArrayPtr<const Triangle>::staticSharedArrayPtr(triangles);

	Polyhedron simpleTetrahedron(verts, triPtr, 4, 4);

	ASSERT_TRUE(simpleTetrahedron.isValid());

	ConvexShapeBuilder builder(verts, triangles, 4, 4, neighBuf, builderRemovalBuffer, builderAddingBuffer);

	builder.addPoint(Vec3f(-1, 10, -1), 3);

	ASSERT_TRUE(builder.toIndexedShape().isValid());

	Vec3f verts2[10]{Vec3f(0.0, 0.0, 0.0), Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 0.0, 1.0), Vec3f(0.0, 1.0, 0.0)};
	Triangle triangles2[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf2[20];


	auto triPtr2 = SharedArrayPtr<const Triangle>::staticSharedArrayPtr(triangles2);

	Polyhedron simpleTetrahedron2(verts2, triPtr2, 4, 4);

	ASSERT_TRUE(simpleTetrahedron2.isValid());

	ConvexShapeBuilder builder2(verts2, triangles2, 4, 4, neighBuf2, builderRemovalBuffer, builderAddingBuffer);

	builder2.addPoint(Vec3f(0.4, 0.4, 0.4), 3);

	ASSERT_TRUE(builder2.toIndexedShape().isValid());

	builder2.addPoint(Vec3f(-0.5, 50, -0.5), 3);

	ASSERT_TRUE(builder2.toIndexedShape().isValid());

	Vec3f newIcosaVerts[30];
	Triangle newIcosaTriangles[40];
	TriangleNeighbors icosaNeighBuf[40];

	ConvexShapeBuilder icosaBuilder(Library::icosahedron, newIcosaVerts, newIcosaTriangles, icosaNeighBuf, builderRemovalBuffer, builderAddingBuffer);

	ASSERT_TRUE(icosaBuilder.toIndexedShape().isValid());

	icosaBuilder.addPoint(Vec3f(0, 1.1, 0));
	icosaBuilder.addPoint(Vec3f(0, -1.1, 0));
	icosaBuilder.addPoint(Vec3f(1.1, 0, 0));
	icosaBuilder.addPoint(Vec3f(-1.1, 0, 0));
	icosaBuilder.addPoint(Vec3f(0, 0, 1.1));
	icosaBuilder.addPoint(Vec3f(0, 0, -1.1));

	ASSERT_TRUE(icosaBuilder.toIndexedShape().isValid());
}
