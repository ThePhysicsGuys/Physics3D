#include "shape.h"

#include <stdlib.h>
#include <cstring>
#include <vector>
#include <set>
#include <math.h>

#include "convexShapeBuilder.h"

#include "../math/utils.h"

#include "../../util/Log.h"

#include "../engineException.h"

#include "../debug.h"
#include "../math/mathUtil.h"

#include "computationBuffer.h"

#include "intersection.h"

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

Shape::Shape() :
	Shape(nullptr, nullptr, nullptr, nullptr, 0, 0) {}

Shape::Shape(Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) :
	Shape(vertices, nullptr, nullptr, triangles, vertexCount, triangleCount) {}

Shape::Shape(Vec3f* vertices, Vec3f* normals, Vec2f* uvs, const Triangle* triangles, int vertexCount, int triangleCount) :
#ifdef __AVX__
	vertices(vertices, vertexCount),
#else
	vertices(vertices),
#endif
	normals(normals), 
	uvs(uvs), 
	triangles(triangles), 
	vertexCount(vertexCount), 
	triangleCount(triangleCount) {}

Shape::Shape(Vec3f* vertices, Vec2f* uvs, const Triangle* triangles, int vertexCount, int triangleCount) :
	Shape(vertices, nullptr, uvs, triangles, vertexCount, triangleCount) {}

Shape::Shape(Vec3f* vertices, Vec3f* normals, const Triangle* triangles, int vertexCount, int triangleCount) : 
	Shape(vertices, normals, nullptr, triangles, vertexCount, triangleCount) {}

NormalizedShape Shape::normalized(Vec3f* vecBuf, Vec3f* normalBuf, CFramef& backTransformation) const {
	backTransformation = getInertialEigenVectors();

	for (int i = 0; i < vertexCount; i++) {
		vecBuf[i] = backTransformation.globalToLocal(vertices[i]);
	}

	if (normals) {
		for (int i = 0; i < vertexCount; i++) {
			normalBuf[i] = CFrame(backTransformation).relativeToLocal(normals.get()[i]);
		}
		return NormalizedShape(vecBuf, normalBuf, uvs.get(), triangles, vertexCount, triangleCount);
	} else {
		return NormalizedShape(vecBuf, nullptr, uvs.get(), triangles, vertexCount, triangleCount);
	}

}

CenteredShape Shape::centered(Vec3f* vecBuf, Vec3& backOffset) const {
	backOffset = getCenterOfMass();

	for(int i = 0; i < vertexCount; i++) {
		vecBuf[i] = vertices[i] - Vec3f(backOffset);
	}

	return CenteredShape(vecBuf, triangles, vertexCount, triangleCount);
}

CFramef Shape::getInertialEigenVectors() const {
	Vec3 centerOfMass = getCenterOfMass();
	SymmetricMat3 inertia = getInertia(centerOfMass);
	Mat3 basis = inertia.getEigenDecomposition().eigenVectors;

	return CFramef(Vec3f(centerOfMass), Mat3f(basis));
}

/* Test */
NormalizedShape::NormalizedShape(Vec3f * vertices, Vec3f * normals, Vec2f * uvs, const Triangle * triangles, int vertexCount, int triangleCount) : CenteredShape(vertices, normals, uvs, triangles, vertexCount, triangleCount) {
	// TODO add normalization verification
};

NormalizedShape::NormalizedShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount) : CenteredShape(vertices, triangles, vertexCount, triangleCount) {
	// TODO add normalization verification
};
/*
	Creates a normalized shape

	Modifies `vertices`
*/
NormalizedShape::NormalizedShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount, CFramef& transformation) : CenteredShape(vertices, triangles, vertexCount, triangleCount) {
	this->normalized(vertices, nullptr, transformation);
}


CenteredShape::CenteredShape(Vec3f * vertices, Vec3f * normals, Vec2f * uvs, const Triangle * triangles, int vertexCount, int triangleCount) : Shape(vertices, normals, uvs, triangles, vertexCount, triangleCount) {

};

CenteredShape::CenteredShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount) : Shape(vertices, triangles, vertexCount, triangleCount) {
	// TODO add centering verification
};

CenteredShape::CenteredShape(Vec3f * vertices, const Triangle * triangles, int vertexCount, int triangleCount, Vec3& offset) : Shape(vertices, triangles, vertexCount, triangleCount) {
	this->centered(vertices, offset);
}

Shape Shape::translated(Vec3f offset, Vec3f * newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = offset + vertices[i];
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

Shape Shape::rotated(RotMat3f rotation, Vec3f* newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = rotation * vertices[i];
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

Shape Shape::localToGlobal(CFramef frame, Vec3f* newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = frame.localToGlobal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

Shape Shape::globalToLocal(CFramef frame, Vec3f* newVecBuf) const {
	for (int i = 0; i < this->vertexCount; i++) {
		newVecBuf[i] = frame.globalToLocal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vertexCount, triangleCount);
}

BoundingBox Shape::getBounds() const {
	double xmin = vertices[0].x, xmax = vertices[0].x;
	double ymin = vertices[0].y, ymax = vertices[0].y;
	double zmin = vertices[0].z, zmax = vertices[0].z;

	for (int i = 1; i < vertexCount; i++) {
		const Vec3f current = vertices[i];

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
bool isComplete(const Triangle* triangles, int triangleCount) {
	for (int i = 0; i < triangleCount; i++) {
		Triangle a = triangles[i];
		
		for (int j = 0; j < triangleCount; j++) {
			if (j == i) continue;

			Triangle b = triangles[j];

			if (a.sharesEdgeWith(b)) {  // correctly oriented
				goto endOfLoop;
			} else if (a.sharesEdgeWith(~b)) {	// wrongly oriented
				Log::warn("triangles[%d](%d, %d, %d) and triangles[%d](%d, %d, %d) are joined wrongly", i, a.firstIndex, a.secondIndex, a.thirdIndex, j, b.firstIndex, b.secondIndex, b.thirdIndex);
				return false;
			}
		}
		Log::warn("No triangle found that shares an edge with triangles[%d]", i);
		return false;
		endOfLoop:;
	}
	return true;
}

bool Shape::isValid() const {
	return isComplete(triangles, triangleCount) && getVolume() >= 0;
}

Vec3f Shape::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3f v0 = vertices[triangle.firstIndex];
	return (vertices[triangle.secondIndex] - v0) % (vertices[triangle.thirdIndex] - v0);
}

void Shape::computeNormals(Vec3f* buffer) const {
	for (int i = 0; i < triangleCount; i++) {
		Triangle triangle = triangles[i];
		Vec3f v0 = vertices[triangle.firstIndex];
		Vec3f v1 = vertices[triangle.secondIndex];
		Vec3f v2 = vertices[triangle.thirdIndex];

		Vec3f D1 = v1 - v0;
		Vec3f D2 = v2 - v0;

		Vec3f faceNormal = (D1 % D2).normalize();

		buffer[triangle.firstIndex] += faceNormal;
		buffer[triangle.secondIndex] += faceNormal;
		buffer[triangle.thirdIndex] += faceNormal;
	}

	for (int i = 0; i < vertexCount; i++) {
		buffer[i] = buffer[i].normalize();
	}
}

bool Shape::containsPoint(Vec3f point) const {
	Vec3f ray(1, 0, 0);

	bool isExiting = false;
	double bestD = static_cast<double>(INFINITY);

	for(const Triangle& tri : iterTriangles()) {
		RayIntersection<float> r = rayTriangleIntersection(point, ray, vertices[tri.firstIndex], vertices[tri.secondIndex], vertices[tri.thirdIndex]);
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
#include <immintrin.h> // I know I don't have to include it, but I'm sending a message!
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

int Shape::furthestIndexInDirection(const Vec3f& direction) const {
	/*float bestDot = vertices[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		float newD = vertices[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;*/

	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	const ParallelVec3& firstBlock = vertices.vecs[0];
	__m256 xTxd = _mm256_mul_ps(dx, firstBlock.xvalues);
	__m256 yTyd = _mm256_mul_ps(dy, firstBlock.yvalues);
	__m256 zTzd = _mm256_mul_ps(dz, firstBlock.zvalues);

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);
	__m256i bestIndices = _mm256_set1_epi32(0);

	for(int blockI = 1; blockI < vertices.blockCount(); blockI++) {
		const ParallelVec3& block = vertices.vecs[blockI];
		__m256i indices = _mm256_set1_epi32(blockI);

		__m256 xTxd = _mm256_mul_ps(dx, block.xvalues);
		__m256 yTyd = _mm256_mul_ps(dy, block.yvalues);
		__m256 zTzd = _mm256_mul_ps(dz, block.zvalues);
		__m256 dot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

		__m256 whichAreMax = _mm256_cmp_ps(dot, bestDot, _CMP_GT_OQ); // Greater than, false if dot == NaN
		bestDot = _mm256_blendv_ps(bestDot, dot, whichAreMax);
		bestIndices = _mm256_blendv_epi32(bestIndices, indices, whichAreMax);
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

Vec3f Shape::furthestInDirection(const Vec3f& direction) const {
	__m256 dx = _mm256_set1_ps(direction.x);
	__m256 dy = _mm256_set1_ps(direction.y);
	__m256 dz = _mm256_set1_ps(direction.z);

	const ParallelVec3& firstBlock = vertices.vecs[0];

	__m256 bestX = firstBlock.xvalues;
	__m256 bestY = firstBlock.yvalues;
	__m256 bestZ = firstBlock.zvalues;

	__m256 xTxd = _mm256_mul_ps(dx, bestX);
	__m256 yTyd = _mm256_mul_ps(dy, bestY);
	__m256 zTzd = _mm256_mul_ps(dz, bestZ);

	__m256 bestDot = _mm256_add_ps(_mm256_add_ps(xTxd, yTyd), zTzd);

	for (int blockI = 1; blockI < vertices.blockCount(); blockI++) {
		const ParallelVec3& block = vertices.vecs[blockI];
		__m256i indices = _mm256_set1_epi32(blockI);

		__m256 xVal = block.xvalues;
		__m256 yVal = block.yvalues;
		__m256 zVal = block.zvalues;

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
	__m256i indxVec = _mm256_castsi128_si256(_mm_cvtsi32_si128(index));

	bestX = _mm256_permutevar8x32_ps(bestX, indxVec);
	bestY = _mm256_permutevar8x32_ps(bestY, indxVec);
	bestZ = _mm256_permutevar8x32_ps(bestZ, indxVec);

	float furthestX = _mm_cvtss_f32(_mm256_castps256_ps128(bestX));
	float furthestY = _mm_cvtss_f32(_mm256_castps256_ps128(bestY));
	float furthestZ = _mm_cvtss_f32(_mm256_castps256_ps128(bestZ));

	return Vec3f(furthestX, furthestY, furthestZ);
}

#else
int Shape::furthestIndexInDirection(const Vec3f& direction) const {
	float bestDot = vertices[0] * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		float newD = vertices[i] * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

Vec3f Shape::furthestInDirection(const Vec3f& direction) const {
	float bestDot = vertices[0] * direction;
	Vec3f bestVertex = vertices[0];
	for (int i = 1; i < vertexCount; i++) {
		float newD = vertices[i] * direction;
		if (newD > bestDot) {
			bestDot = newD;
			bestVertex = vertices[i];
		}
	}

	return bestVertex;
}
#endif




ComputationBuffers buffers(1000, 2000);
bool Shape::intersects(const Shape& other, Vec3f& intersection, Vec3f& exitVector, const Vec3& centerConnection) const {
	Tetrahedron result;
	bool collides = runGJK(*this, other, Vec3f(centerConnection), result);
	
	if(collides) {
		bool epaResult = runEPA(*this, other, result, intersection, exitVector, buffers);
		return epaResult;
	} else {
		return false;
	}
}

#include "../physicsProfiler.h"

bool Shape::intersectsTransformed(const Shape& other, const CFramef& relativeCFramef, Vec3f& intersection, Vec3f& exitVector) const {
	Tetrahedron result;
	bool collides = runGJKTransformed(*this, other, relativeCFramef, relativeCFramef.position, result);
	
	if(collides) {
		return runEPATransformed(*this, other, result, relativeCFramef, intersection, exitVector, buffers);
	} else {
		return false;
	}
}

double Shape::getVolume() const {
	double total = 0;
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = vertices[triangle.firstIndex]; 
		Vec3f v1 = vertices[triangle.secondIndex];
		Vec3f v2 = vertices[triangle.thirdIndex];

		Vec3f D1 = v1 - v0; 
		Vec3f D2 = v2 - v0;
		
		double Tf = (D1.x * D2.y - D1.y * D2.x);

		total += Tf * ((D1.z + D2.z) / 6 + v0.z / 2);
	}

	return total;
}

Vec3 Shape::getCenterOfMass() const {
	Vec3 total(0,0,0);
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = vertices[triangle.firstIndex];
		Vec3f v1 = vertices[triangle.secondIndex];
		Vec3f v2 = vertices[triangle.thirdIndex];

		Vec3f D1 = v1 - v0;
		Vec3f D2 = v2 - v0;

		Vec3f dFactor = D1 % D2;
		Vec3f vFactor = v0.squared() + v1.squared() + v2.squared() + v0.mul(v1) + v1.mul(v2) + v2.mul(v0);

		total += Vec3(dFactor.mul(vFactor));
	}
	
	return total / (24 * getVolume());
}

Sphere Shape::getCircumscribedSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = Vec3(bounds.xmax + bounds.xmin, bounds.ymax + bounds.ymin, bounds.zmax + bounds.zmin) / 2.0;
	double radius = (Vec3(bounds.xmax, bounds.ymax, bounds.zmax) - center).length();
	return Sphere{center, radius};
}

double Shape::getMaxRadius() const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = vertex.lengthSquared();
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return sqrt(bestDistSq);
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
SymmetricMat3 Shape::getInertia(CFrame reference) const {
	SymmetricMat3 total(0, 0, 0, 0, 0, 0);
	for (Triangle triangle : iterTriangles()) {
		Vec3 v0 = reference.globalToLocal(Vec3(vertices[triangle.firstIndex]));
		Vec3 v1 = reference.globalToLocal(Vec3(vertices[triangle.secondIndex]));
		Vec3 v2 = reference.globalToLocal(Vec3(vertices[triangle.thirdIndex]));

		Vec3 D1 = v1 - v0;
		Vec3 D2 = v2 - v0;

		Vec3 dFactor = D1 % D2;

		// Diagonal Elements
		Vec3 squaredIntegral = v0.cubed() + v1.cubed() + v2.cubed() + v0.squared().mul(v1 + v2) + v1.squared().mul(v0 + v2) + v2.squared().mul(v0 + v1) + v0.mul(v1).mul(v2);
		Vec3 diagonalElementParts = dFactor.mul(squaredIntegral) / 60;

		total.m00 += diagonalElementParts.y + diagonalElementParts.z;
		total.m11 += diagonalElementParts.z + diagonalElementParts.x;
		total.m22 += diagonalElementParts.x + diagonalElementParts.y;

		// Other Elements
		double selfProducts  =	v0.x*v0.y*v0.z + v1.x*v1.y*v1.z + v2.x*v2.y*v2.z;
		double twoSames      =	v0.x*v0.y*v1.z + v0.x*v1.y*v0.z + v0.x*v1.y*v1.z + v0.x*v0.y*v2.z + v0.x*v2.y*v0.z + v0.x*v2.y*v2.z +
								v1.x*v0.y*v0.z + v1.x*v1.y*v0.z + v1.x*v0.y*v1.z + v1.x*v1.y*v2.z + v1.x*v2.y*v1.z + v1.x*v2.y*v2.z +
								v2.x*v0.y*v0.z + v2.x*v1.y*v2.z + v2.x*v0.y*v2.z + v2.x*v1.y*v1.z + v2.x*v2.y*v0.z + v2.x*v2.y*v1.z;
		double allDifferents =	v0.x*v1.y*v2.z + v0.x*v2.y*v1.z + v1.x*v0.y*v2.z + v1.x*v2.y*v0.z + v2.x*v0.y*v1.z + v2.x*v1.y*v0.z;

		double xyzIntegral = -(3 * selfProducts + twoSames + allDifferents / 2) / 60;

		total.m01 += dFactor.z * xyzIntegral;
		total.m02 += dFactor.y * xyzIntegral;
		total.m12 += dFactor.x * xyzIntegral;
	}
	
	return total;
}

SymmetricMat3 Shape::getInertia(Vec3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Shape::getInertia(Mat3 reference) const {
	return this->getInertia(CFrame(reference));
}

SymmetricMat3 Shape::getInertia() const {
	return this->getInertia(CFrame());
}

float Shape::getIntersectionDistance(Vec3f origin, Vec3f direction) {
	const float EPSILON = 0.0000001f;
	float t = INFINITY;
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = vertices[triangle.firstIndex];
		Vec3f v1 = vertices[triangle.secondIndex];
		Vec3f v2 = vertices[triangle.thirdIndex];

		Vec3f edge1, edge2, h, s, q;
		float a, f, u, v;

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		h = direction % edge2;
		a = edge1 * h;

		if (a > -EPSILON && a < EPSILON) continue;   
		
		f = 1.0 / a;
		s = origin - v0;
		u = f * (s * h);

		if (u < 0.0 || u > 1.0) continue;
		
		q = s % edge1;
		v = direction * f * q;

		if (v < 0.0 || u + v > 1.0) continue;

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
