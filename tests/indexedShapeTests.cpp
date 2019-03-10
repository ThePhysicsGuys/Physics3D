#include "testsMain.h"
#include "../engine/geometry/indexedShape.h"
#include "../engine/geometry/shapeBuilder.h"

TEST_CASE(testIndexedShape) {
	Vec3 verts[]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles[]{{0,2,1},{0,1,3},{0,3,2},{1,2,3}};

	TriangleNeighbors neighBuf[4];

	IndexedShape s(verts, triangles, 4, 4, neighBuf);

	ASSERT_TRUE(s.isValid());
}

TEST_CASE(buildShape) {
	Vec3 verts[10]{Vec3(0.0, 0.0, 0.0), Vec3(1.0, 0.0, 0.0), Vec3(0.0, 0.0, 1.0), Vec3(0.0, 1.0, 0.0)};
	Triangle triangles[20]{{0,2,1},{0,1,3},{0,3,2},{1,2,3}};
	TriangleNeighbors neighBuf[20];

	ShapeBuilder b(verts, triangles, 4, 4, neighBuf);

	b.addPoint(Vec3(1, 1, 1), 3);

	ASSERT_TRUE(b.toIndexedShape().isValid());
}
