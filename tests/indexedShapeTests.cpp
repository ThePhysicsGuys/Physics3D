#include "testsMain.h"
#include "../engine/geometry/indexedShape.h"
#include "../engine/geometry/shapeBuilder.h"
#include "../engine/geometry/convexShapeBuilder.h"
#include "../application/objectLibrary.h"

TEST_CASE(testIndexedShape) {
	Vec3 verts[]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles[]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};

	TriangleNeighbors neighBuf[4];

	fillNeighborBuf(triangles, 4, neighBuf);

	IndexedShape s(verts, triangles, 4, 4, neighBuf);

	ASSERT_TRUE(s.isValid());
}

TEST_CASE(buildShape) {
	Vec3 verts[10]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles[10]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	ShapeBuilder b(verts, triangles, 4, 4, neighBuf);

	b.addPoint(Vec3(1, 1, 1), 3);

	ASSERT_TRUE(b.toIndexedShape().isValid());
}

TEST_CASE(buildConvexShape) {
	int builderRemovalBuffer[1000];
	EdgePiece builderAddingBuffer[1000];

	Vec3 verts[10]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf[20];

	Shape simpleTetrahedron(verts, triangles, 4, 4);

	ASSERT_TRUE(simpleTetrahedron.isValid());

	ConvexShapeBuilder builder(verts, triangles, 4, 4, neighBuf, builderRemovalBuffer, builderAddingBuffer);

	builder.addPoint(Vec3(-1, 10, -1), 3);

	ASSERT_TRUE(builder.toIndexedShape().isValid());

	Vec3 verts2[10]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles2[20]{{0,1,2},{0,3,1},{0,2,3},{1,3,2}};
	TriangleNeighbors neighBuf2[20];

	Shape simpleTetrahedron2(verts2, triangles2, 4, 4);

	ASSERT_TRUE(simpleTetrahedron2.isValid());

	ConvexShapeBuilder builder2(verts2, triangles2, 4, 4, neighBuf2, builderRemovalBuffer, builderAddingBuffer);

	builder2.addPoint(Vec3(0.4, 0.4, 0.4), 3);

	ASSERT_TRUE(builder2.toIndexedShape().isValid());

	builder2.addPoint(Vec3(-0.5, 50, -0.5), 3);

	ASSERT_TRUE(builder2.toIndexedShape().isValid());

	Vec3 newIcosaVerts[30];
	Triangle newIcosaTriangles[40];
	TriangleNeighbors icosaNeighBuf[40];

	ConvexShapeBuilder icosaBuilder(icosahedron, newIcosaVerts, newIcosaTriangles, icosaNeighBuf, builderRemovalBuffer, builderAddingBuffer);

	ASSERT_TRUE(icosaBuilder.toIndexedShape().isValid());

	icosaBuilder.addPoint(Vec3(0, 1.5, 0));
	icosaBuilder.addPoint(Vec3(0, -1.5, 0));
	icosaBuilder.addPoint(Vec3(1.5, 0, 0));
	icosaBuilder.addPoint(Vec3(-1.5, 0, 0));
	icosaBuilder.addPoint(Vec3(0, 0, 1.5));
	icosaBuilder.addPoint(Vec3(0, 0, -1.5));

	ASSERT_TRUE(icosaBuilder.toIndexedShape().isValid());
}
