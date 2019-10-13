#include "polyhedron.h"

#include <stdlib.h>
#include <cstring>
#include <vector>
#include <set>
#include <math.h>

#include "convexShapeBuilder.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/trigonometry.h"
#include "../math/utils.h"
#include "../math/mathUtil.h"
#include "../../util/Log.h"
#include "../debug.h"
#include "../physicsProfiler.h"
#include "computationBuffer.h"
#include "intersection.h"

#include "../physicsProfiler.h"

size_t getOffset(size_t size) {
	return (size + 7) & 0xFFFFFFFFFFFFFFF8;
}

inline SharedAlignedPointer<float> createParallelVecBuf(size_t size) {
	return SharedAlignedPointer<float>(getOffset(size) * 3, 32);
}
inline SharedAlignedPointer<int> createParallelTriangleBuf(size_t size) {
	return SharedAlignedPointer<int>(getOffset(size) * 3, 32);
}

template<typename T>
inline void fixFinalBlock(T* buf, size_t size) {
	size_t offset = getOffset(size);
	T* xValues = buf;
	T* yValues = buf + offset;
	T* zValues = buf + 2 * offset;

	for (size_t i = size; i < offset; i++) {
		xValues[i] = xValues[size - 1];
		yValues[i] = yValues[size - 1];
		zValues[i] = zValues[size - 1];
	}
}

inline SharedAlignedPointer<float> createAndFillParallelVecBuf(size_t size, const Vec3f* vectors) {
	SharedAlignedPointer<float> buf = createParallelVecBuf(size);

	size_t offset = getOffset(size);

	float* xValues = buf;
	float* yValues = buf + offset;
	float* zValues = buf + 2 * offset;

	for (size_t i = 0; i < size; i++) {
		xValues[i] = vectors[i].x;
		yValues[i] = vectors[i].y;
		zValues[i] = vectors[i].z;
	}
	fixFinalBlock(buf.get(), size);

	return buf;
}

inline SharedAlignedPointer<int> createAndFillParallelTriangleBuf(size_t size, const Triangle* triangles) {
	SharedAlignedPointer<int> buf = createParallelTriangleBuf(size);

	size_t offset = getOffset(size);

	int* aValues = buf;
	int* bValues = buf + offset;
	int* cValues = buf + 2 * offset;

	for (size_t i = 0; i < size; i++) {
		aValues[i] = triangles[i].firstIndex;
		bValues[i] = triangles[i].secondIndex;
		cValues[i] = triangles[i].thirdIndex;
	}
	fixFinalBlock(buf.get(), size);

	return buf;
}


inline void setInBuf(float* buf, size_t size, size_t index, const Vec3f& value) {
	size_t offset = getOffset(size);

	float* xValues = buf;
	float* yValues = buf + offset;
	float* zValues = buf + 2 * offset;

	xValues[index] = value.x;
	yValues[index] = value.y;
	zValues[index] = value.z;
}


bool Triangle::sharesEdgeWith(Triangle other) const {
	return firstIndex == other.secondIndex && secondIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.secondIndex ||
		firstIndex == other.firstIndex && secondIndex == other.thirdIndex || 

		secondIndex == other.secondIndex && thirdIndex == other.firstIndex ||
		secondIndex == other.thirdIndex && thirdIndex == other.secondIndex ||
		secondIndex == other.firstIndex && thirdIndex == other.thirdIndex ||

		thirdIndex == other.secondIndex && firstIndex == other.firstIndex ||
		thirdIndex == other.thirdIndex && firstIndex == other.secondIndex ||
		thirdIndex == other.firstIndex && firstIndex == other.thirdIndex;
}

Triangle Triangle::operator~() const {
	return Triangle { firstIndex, thirdIndex, secondIndex };
}

bool Triangle::operator==(const Triangle & other) const {
	return firstIndex == other.firstIndex && secondIndex == other.secondIndex && thirdIndex == other.thirdIndex || 
		firstIndex == other.secondIndex && secondIndex == other.thirdIndex && thirdIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.firstIndex && thirdIndex == other.secondIndex;
}

Triangle Triangle::rightShift() const { 
	return Triangle { thirdIndex, firstIndex, secondIndex };
}

Triangle Triangle::leftShift() const {
	return Triangle { secondIndex, thirdIndex, firstIndex };
}

const float* copyVerts(const float* verts, size_t vertexCount) {
	float* buf = createParallelVecBuf(vertexCount);
	for (size_t i = 0; i < getOffset(vertexCount) * 3; i++) {
		buf[i] = verts[i];
	}
	return buf;
}

const float* Polyhedron::copyOfVerts() const {
	return copyVerts(this->vertices, this->vertexCount);
}

inline Polyhedron::Polyhedron(const SharedAlignedPointer<float>& vertices, const SharedAlignedPointer<int>& triangles, int vertexCount, int triangleCount) :
	vertices(vertices), triangles(triangles), vertexCount(vertexCount), triangleCount(triangleCount) {
}

Polyhedron::~Polyhedron() {
	
}

Polyhedron::Polyhedron(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) :
	vertices(createAndFillParallelVecBuf(vertexCount, vertices)),
	triangles(createAndFillParallelTriangleBuf(triangleCount, triangles)),
	vertexCount(vertexCount), 
	triangleCount(triangleCount) {}

/*CFramef Polyhedron::getInertialEigenVectors() const {
	Vec3 centerOfMass = getCenterOfMass();
	SymmetricMat3 inertia = getInertia(centerOfMass);
	Mat3 basis = inertia.getEigenDecomposition().eigenVectors;

	return CFramef(Vec3f(centerOfMass), Mat3f(basis));
}*/

Polyhedron Polyhedron::translated(Vec3f offset) const {
	SharedAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, (*this)[i] + offset);
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(newBuf, triangles, vertexCount, triangleCount);
}

Polyhedron Polyhedron::rotated(RotMat3f rotation) const {
	SharedAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, rotation * (*this)[i]);
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(newBuf, triangles, vertexCount, triangleCount);
}

Polyhedron Polyhedron::localToGlobal(CFramef frame) const {
	SharedAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, frame.localToGlobal((*this)[i]));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(newBuf, triangles, vertexCount, triangleCount);
}

Polyhedron Polyhedron::globalToLocal(CFramef frame) const {
	SharedAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		setInBuf(newBuf, vertexCount, i, frame.globalToLocal((*this)[i]));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(newBuf, triangles, vertexCount, triangleCount);
}
Polyhedron Polyhedron::scaled(float scaleX, float scaleY, float scaleZ) const {
	SharedAlignedPointer<float> newBuf = createParallelVecBuf(this->vertexCount);
	for (int i = 0; i < this->vertexCount; i++) {
		Vec3f v = (*this)[i];
		setInBuf(newBuf, vertexCount, i, Vec3f(scaleX * v.x, scaleY * v.y, scaleZ * v.z));
	}

	fixFinalBlock(newBuf.get(), this->vertexCount);
	return Polyhedron(newBuf, triangles, vertexCount, triangleCount);
}
Polyhedron Polyhedron::scaled(double scaleX, double scaleY, double scaleZ) const {
	return scaled(static_cast<float>(scaleX), static_cast<float>(scaleY), static_cast<float>(scaleZ));
}

BoundingBox Polyhedron::getBounds() const {
	double xmin = (*this)[0].x, xmax = (*this)[0].x;
	double ymin = (*this)[0].y, ymax = (*this)[0].y;
	double zmin = (*this)[0].z, zmax = (*this)[0].z;

	for (int i = 1; i < vertexCount; i++) {
		const Vec3f current = (*this)[i];

		if (current.x < xmin) xmin = current.x;
		if (current.x > xmax) xmax = current.x;
		if (current.y < ymin) ymin = current.y;
		if (current.y > ymax) ymax = current.y;
		if (current.z < zmin) zmin = current.z;
		if (current.z > zmax) zmax = current.z;
	}

	return BoundingBox { xmin, ymin, zmin, xmax, ymax, zmax };
}

// for every edge, of every triangle, check that it coincides with exactly one other triangle, in reverse order
bool isComplete(ShapeTriangleIter iter, ShapeTriangleIter fin) {
	for (; iter != fin; ++iter) {
		Triangle a = *iter;

		ShapeTriangleIter iter2 = iter;
		++iter2;
		for (; iter2 != fin; ++iter2) {
			Triangle b = *iter2;

			if (a.sharesEdgeWith(b)) {  // correctly oriented
				goto endOfLoop;
			} else if (a.sharesEdgeWith(~b)) {	// wrongly oriented
				Log::warn("triangle (%d, %d, %d) and triangle (%d, %d, %d) are joined wrongly", a.firstIndex, a.secondIndex, a.thirdIndex, b.firstIndex, b.secondIndex, b.thirdIndex);
				return false;
			}
		}
		Log::warn("No triangle found that shares an edge with triangle(%d, %d, %d)", a.firstIndex, a.secondIndex, a.thirdIndex);
		return false;
		endOfLoop:;
	}
	return true;
}

bool Polyhedron::isValid() const {
	IteratorFactory<ShapeTriangleIter> f = iterTriangles();
	return isComplete(f.begin(), f.end()) && getVolume() >= 0;
}

Vec3f Polyhedron::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3f v0 = (*this)[triangle.firstIndex];
	return ((*this)[triangle.secondIndex] - v0) % ((*this)[triangle.thirdIndex] - v0);
}


// TODO parallelize
void Polyhedron::computeNormals(Vec3f* buffer) const {
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3f D1 = v1 - v0;
		Vec3f D2 = v2 - v0;

		Vec3f faceNormal = normalize(D1 % D2);

		buffer[triangle.firstIndex] += faceNormal;
		buffer[triangle.secondIndex] += faceNormal;
		buffer[triangle.thirdIndex] += faceNormal;
	}

	for (int i = 0; i < vertexCount; i++) {
		buffer[i] = normalize(buffer[i]);
	}
}

//TODO parallelize
bool Polyhedron::containsPoint(Vec3f point) const {
	Vec3f ray(1, 0, 0);

	bool isExiting = false;
	double bestD = static_cast<double>(INFINITY);

	for(const Triangle& tri : iterTriangles()) {
		RayIntersection<float> r = rayTriangleIntersection(point, ray, (*this)[tri.firstIndex], (*this)[tri.secondIndex], (*this)[tri.thirdIndex]);
		if(r.d >= 0 && r.lineIntersectsTriangle()) {
			if(r.d < bestD) {
				bestD = r.d;
				isExiting = (getNormalVecOfTriangle(tri) * ray >= 0);
			}
		}
	}

	return isExiting;
}



#ifdef __AVX__
#include <immintrin.h>
#ifdef _MSC_VER
inline uint32_t __builtin_ctz(uint32_t x) {
	unsigned long ret;
	_BitScanForward(&ret, x);
	return (int)ret;
}
#endif
inline __m256i _mm256_blendv_epi32(__m256i a, __m256i b, __m256 mask) {
	return _mm256_castps_si256(
		_mm256_blendv_ps(
			_mm256_castsi256_ps(a),
			_mm256_castsi256_ps(b),
			mask
		)
	);
}

inline uint32_t mm256_extract_epi32_var_indx(__m256i vec, int i)
{
	__m128i indx = _mm_cvtsi32_si128(i);
	__m256i val = _mm256_permutevar8x32_epi32(vec, _mm256_castsi128_si256(indx));
	return         _mm_cvtsi128_si32(_mm256_castsi256_si128(val));
}

int Polyhedron::furthestIndexInDirection(const Vec3f& direction) const {
	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	size_t offset = getOffset(this->vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 xTxd = _mm256_mul_ps(dx, _mm256_load_ps(xValues));
	__m256 yTyd = _mm256_mul_ps(dy, _mm256_load_ps(yValues));
	__m256 zTzd = _mm256_mul_ps(dz, _mm256_load_ps(zValues));

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);
	__m256i bestIndices = _mm256_set1_epi32(0);

	for(int blockI = 1; blockI < (vertexCount+7)/8; blockI++) {
		__m256i indices = _mm256_set1_epi32(blockI);

		__m256 xTxd = _mm256_mul_ps(dx, _mm256_load_ps(xValues + blockI * 8));
		__m256 yTyd = _mm256_mul_ps(dy, _mm256_load_ps(yValues + blockI * 8));
		__m256 zTzd = _mm256_mul_ps(dz, _mm256_load_ps(zValues + blockI * 8));
		__m256 dot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

		__m256 whichAreMax = _mm256_cmp_ps(dot, bestDot, _CMP_GT_OQ); // Greater than, false if dot == NaN
		bestDot = _mm256_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm256_blendv_epi32(bestIndices, indices, whichAreMax); // TODO convert to _mm256_blendv_epi8
	}
	// find max of our 8 left candidates
	__m256 swap4x4 = _mm256_permute2f128_ps(bestDot, bestDot, 1);
	__m256 bestDotInternalMax = _mm256_max_ps(bestDot, swap4x4);
	__m256 swap2x2 = _mm256_permute_ps(bestDotInternalMax, 0b01001110);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap2x2);
	__m256 swap1x1 = _mm256_permute_ps(bestDotInternalMax, 0b10110001);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap1x1);

	__m256 compare = _mm256_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_OQ);
	uint32_t mask = _mm256_movemask_ps(compare);
	uint32_t index = __builtin_ctz(mask);
	uint32_t block = mm256_extract_epi32_var_indx(bestIndices, index);
	return block * 8 + index;
}

Vec3f Polyhedron::furthestInDirection(const Vec3f& direction) const {
	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	size_t offset = getOffset(this->vertexCount);
	const float* xValues = this->vertices;
	const float* yValues = this->vertices + offset;
	const float* zValues = this->vertices + 2 * offset;

	__m256 bestX = _mm256_load_ps(xValues);
	__m256 bestY = _mm256_load_ps(yValues);
	__m256 bestZ = _mm256_load_ps(zValues);

	__m256 xTxd = _mm256_mul_ps(dx, bestX);
	__m256 yTyd = _mm256_mul_ps(dy, bestY);
	__m256 zTzd = _mm256_mul_ps(dz, bestZ);

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

	for (int blockI = 1; blockI < (vertexCount + 7) / 8; blockI++) {
		__m256i indices = _mm256_set1_epi32(blockI);

		__m256 xVal = _mm256_load_ps(xValues + blockI * 8);
		__m256 yVal = _mm256_load_ps(yValues + blockI * 8);
		__m256 zVal = _mm256_load_ps(zValues + blockI * 8);

		__m256 xTxd = _mm256_mul_ps(dx, xVal);
		__m256 yTyd = _mm256_mul_ps(dy, yVal);
		__m256 zTzd = _mm256_mul_ps(dz, zVal);
		__m256 dot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

		__m256 whichAreMax = _mm256_cmp_ps(dot, bestDot, _CMP_GT_OQ); // Greater than, false if dot == NaN
		bestDot = _mm256_blendv_ps(bestDot, dot, whichAreMax);
		bestX = _mm256_blendv_ps(bestX, xVal, whichAreMax);
		bestY = _mm256_blendv_ps(bestY, yVal, whichAreMax);
		bestZ = _mm256_blendv_ps(bestZ, zVal, whichAreMax);
	}

	// now we find the max of the remaining 8 elements
	__m256 swap4x4 = _mm256_permute2f128_ps(bestDot, bestDot, 1);
	__m256 bestDotInternalMax = _mm256_max_ps(bestDot, swap4x4);
	__m256 swap2x2 = _mm256_permute_ps(bestDotInternalMax, 0b01001110);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap2x2);
	__m256 swap1x1 = _mm256_permute_ps(bestDotInternalMax, 0b10110001);
	bestDotInternalMax = _mm256_max_ps(bestDotInternalMax, swap1x1);

	__m256 compare = _mm256_cmp_ps(bestDotInternalMax, bestDot, _CMP_EQ_OQ);
	uint32_t mask = _mm256_movemask_ps(compare);
	uint32_t index = __builtin_ctz(mask);
	
	return Vec3f(bestX.m256_f32[index], bestY.m256_f32[index], bestZ.m256_f32[index]);
}

#else
int Polyhedron::furthestIndexInDirection(const Vec3f& direction) const {
	float bestDot = (*this)[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		float newD = (*this)[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

Vec3f Polyhedron::furthestInDirection(const Vec3f& direction) const {
	float bestDot = (*this)[0] * direction;
	Vec3f bestVertex = (*this)[0];
	for (int i = 1; i < vertexCount; i++) {
		float newD = (*this)[i] * direction;
		if (newD > bestDot) {
			bestDot = newD;
			bestVertex = (*this)[i];
		}
	}

	return bestVertex;
}
#endif

template<int N>
void incDebugTally(HistoricTally<N, long long, IterationTime>& tally, int iterTime) {
	if (iterTime >= 200) {
		tally.addToTally(IterationTime::LIMIT_REACHED, 1);
	} else if (iterTime >= 15) {
		tally.addToTally(IterationTime::TOOMANY, 1);
	} else {
		tally.addToTally(static_cast<IterationTime>(iterTime), 1);
	}
}

ComputationBuffers buffers(1000, 2000);

bool Polyhedron::intersectsTransformed(const Polyhedron& other, const CFramef& relativeCFramef, Vec3f& intersection, Vec3f& exitVector) const {
	Tetrahedron result;
	int iter;
	physicsMeasure.mark(PhysicsProcess::GJK_COL);
	bool collides = runGJKTransformed(*this, other, relativeCFramef, -relativeCFramef.position, result, iter);
	
	if(collides) {
		incDebugTally(GJKCollidesIterationStatistics, iter + 2);
		physicsMeasure.mark(PhysicsProcess::EPA);
		bool epaResult = runEPATransformed(*this, other, result, relativeCFramef, intersection, exitVector, buffers, iter);
		incDebugTally(EPAIterationStatistics, iter);
		return epaResult;
	} else {
		incDebugTally(GJKNoCollidesIterationStatistics, iter + 2);
		physicsMeasure.mark(PhysicsProcess::OTHER, PhysicsProcess::GJK_NO_COL);
		return false;
	}
}

double Polyhedron::getVolume() const {
	double total = 0;
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3 D1 = v1 - v0; 
		Vec3 D2 = v2 - v0;
		
		double Tf = (D1.x * D2.y - D1.y * D2.x);

		total += Tf * ((D1.z + D2.z) / 6 + v0.z / 2);
	}

	return total;
}

Vec3 Polyhedron::getCenterOfMass() const {
	Vec3 total(0,0,0);
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3f D1 = v1 - v0;
		Vec3f D2 = v2 - v0;

		Vec3f dFactor = D1 % D2;
		Vec3f vFactor = elementWiseSquare(v0) + elementWiseSquare(v1) + elementWiseSquare(v2) + elementWiseMul(v0, v1) + elementWiseMul(v1, v2) + elementWiseMul(v2, v0);

		total += Vec3(elementWiseMul(dFactor, vFactor));
	}
	
	return total / (24 * getVolume());
}

CircumscribingSphere Polyhedron::getCircumscribingSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = Vec3(bounds.xmax + bounds.xmin, bounds.ymax + bounds.ymin, bounds.zmax + bounds.zmin) / 2.0;
	double radius = getMaxRadius(center);
	return CircumscribingSphere{center, radius};
}

double Polyhedron::getMaxRadiusSq() const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex);
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double Polyhedron::getMaxRadiusSq(Vec3f reference) const {
	double bestDistSq = 0;
	for (Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex-reference);
		if (distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double Polyhedron::getMaxRadius() const {
	return sqrt(getMaxRadiusSq());
}

double Polyhedron::getMaxRadius(Vec3f reference) const {
	return sqrt(getMaxRadiusSq(reference));
}

/*
	The total inertial matrix is given by the integral over the volume of the shape of the following matrix:
	[[
	[y^2+z^2,    xy,    xz],
	[xy,    x^2+z^2,    yz],
	[xz,    yz,    x^2+y^2]
	]]

	This has been reworked to a surface integral resulting in the given formulae
*/
SymmetricMat3 Polyhedron::getInertia(CFrame reference) const {
	SymmetricMat3 total(SymmetricMat3::ZEROS());
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = reference.globalToLocal(Vec3((*this)[triangle.firstIndex]));
		Vec3 v1 = reference.globalToLocal(Vec3((*this)[triangle.secondIndex]));
		Vec3 v2 = reference.globalToLocal(Vec3((*this)[triangle.thirdIndex]));

		Vec3 D1 = v1 - v0;
		Vec3 D2 = v2 - v0;

		Vec3 dFactor = D1 % D2;

		// Diagonal Elements
		Vec3 squaredIntegral = elementWiseCube(v0) + elementWiseCube(v1) + elementWiseCube(v2) + elementWiseMul(elementWiseSquare(v0), v1 + v2) + elementWiseMul(elementWiseSquare(v1), v0 + v2) + elementWiseMul(elementWiseSquare(v2), v0 + v1) + elementWiseMul(elementWiseMul(v0, v1), v2);
		Vec3 diagonalElementParts = elementWiseMul(dFactor, squaredIntegral) / 60;

		total[0][0] += diagonalElementParts.y + diagonalElementParts.z;
		total[1][1] += diagonalElementParts.z + diagonalElementParts.x;
		total[2][2] += diagonalElementParts.x + diagonalElementParts.y;

		// Other Elements
		double selfProducts  =	v0.x*v0.y*v0.z + v1.x*v1.y*v1.z + v2.x*v2.y*v2.z;
		double twoSames      =	v0.x*v0.y*v1.z + v0.x*v1.y*v0.z + v0.x*v1.y*v1.z + v0.x*v0.y*v2.z + v0.x*v2.y*v0.z + v0.x*v2.y*v2.z +
								v1.x*v0.y*v0.z + v1.x*v1.y*v0.z + v1.x*v0.y*v1.z + v1.x*v1.y*v2.z + v1.x*v2.y*v1.z + v1.x*v2.y*v2.z +
								v2.x*v0.y*v0.z + v2.x*v1.y*v2.z + v2.x*v0.y*v2.z + v2.x*v1.y*v1.z + v2.x*v2.y*v0.z + v2.x*v2.y*v1.z;
		double allDifferents =	v0.x*v1.y*v2.z + v0.x*v2.y*v1.z + v1.x*v0.y*v2.z + v1.x*v2.y*v0.z + v2.x*v0.y*v1.z + v2.x*v1.y*v0.z;

		double xyzIntegral = -(3 * selfProducts + twoSames + allDifferents / 2) / 60;

		total[1][0] += dFactor.z * xyzIntegral;
		total[2][0] += dFactor.y * xyzIntegral;
		total[2][1] += dFactor.x * xyzIntegral;
	}
	
	return total;
}

SymmetricMat3 Polyhedron::getInertia(Vec3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Polyhedron::getInertia(Mat3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Polyhedron::getInertia() const {
	return this->getInertia(CFrame());
}

void Polyhedron::getCircumscribedEllipsoid() const {

}

float Polyhedron::getIntersectionDistance(Vec3f origin, Vec3f direction) const {
	const float EPSILON = 0.0000001f;
	float t = INFINITY;
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = (*this)[triangle.firstIndex];
		Vec3f v1 = (*this)[triangle.secondIndex];
		Vec3f v2 = (*this)[triangle.thirdIndex];

		Vec3f edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		h = direction % edge2;
		a = edge1 * h;

		if (a > -EPSILON && a < EPSILON) continue;   
		
		f = 1.0f / a;
		s = origin - v0;
		u = f * (s * h);

		if (u < 0.0 || u > 1.0) continue;
		
		q = s % edge1;
		v = direction * f * q;

		if (v < 0.0f || u + v > 1.0f) continue;

		float r = edge2 * f * q;
		if (r > EPSILON) { 
			if (r < t) t = r;
		} else {
			//Log::debug("Line intersection but not a ray intersection");
			continue;
		}
	}

	return t;
}
