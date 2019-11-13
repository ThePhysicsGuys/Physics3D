#include "polyhedronInternals.h"

#include "polyhedron.h"

UniqueAlignedPointer<float> createAndFillParallelVecBuf(size_t size, const Vec3f* vectors) {
	UniqueAlignedPointer<float> buf = createParallelVecBuf(size);

	size_t offset = getOffset(size);

	float* xValues = buf;
	float* yValues = buf + offset;
	float* zValues = buf + 2 * offset;

	for(size_t i = 0; i < size; i++) {
		xValues[i] = vectors[i].x;
		yValues[i] = vectors[i].y;
		zValues[i] = vectors[i].z;
	}
	fixFinalBlock(buf.get(), size);

	return buf;
}

UniqueAlignedPointer<int> createAndFillParallelTriangleBuf(size_t size, const Triangle* triangles) {
	UniqueAlignedPointer<int> buf = createParallelTriangleBuf(size);

	size_t offset = getOffset(size);

	int* aValues = buf;
	int* bValues = buf + offset;
	int* cValues = buf + 2 * offset;

	for(size_t i = 0; i < size; i++) {
		aValues[i] = triangles[i].firstIndex;
		bValues[i] = triangles[i].secondIndex;
		cValues[i] = triangles[i].thirdIndex;
	}
	fixFinalBlock(buf.get(), size);

	return buf;
}


void setInBuf(float* buf, size_t size, size_t index, const Vec3f& value) {
	size_t offset = getOffset(size);

	float* xValues = buf;
	float* yValues = buf + offset;
	float* zValues = buf + 2 * offset;

	xValues[index] = value.x;
	yValues[index] = value.y;
	zValues[index] = value.z;
}
