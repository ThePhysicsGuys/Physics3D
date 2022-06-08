#include "triangleMesh.h"

#include "../misc/validityHelper.h"
#include "../misc/cpuid.h"

#include <cstdio>
#include <math.h>
#include <vector>
#include <set>
#include <cmath>

namespace P3D {
#pragma region bufManagement
static size_t getOffset(size_t size) {
        return (size + 7) & 0xFFFFFFFFFFFFFFF8;
 

}
static UniqueAlignedPointer<float> createParallelVecBuf(size_t size) {
	return UniqueAlignedPointer<float>(getOffset(size) * 3, 32);
}
static UniqueAlignedPointer<int> createParallelTriangleBuf(size_t size) {
	return UniqueAlignedPointer<int>(getOffset(size) * 3, 32);
}

template<typename T>
static UniqueAlignedPointer<T> copy(const UniqueAlignedPointer<T>& buf, size_t size) {
	size_t totalBufSize = getOffset(size) * 3;
	UniqueAlignedPointer<T> result(totalBufSize, 32);

	for(size_t i = 0; i < totalBufSize; i++) {
		result[i] = buf[i];
	}

	return result;
}

template<typename T>
static void fixFinalBlock(T* buf, size_t size) {
	size_t offset = ((size + 7) & 0xFFFFFFFFFFFFFFF8);
	T* xValues = buf;
	T* yValues = buf + offset;
	T* zValues = buf + 2 * offset;

	for(size_t i = size; i < offset; i++) {
		xValues[i] = xValues[size - 1];
		yValues[i] = yValues[size - 1];
                zValues[i] = zValues[size - 1];
	}
}
#pragma endregion

#pragma region triangle
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

bool Triangle::operator==(const Triangle& other) const {
	return firstIndex == other.firstIndex && secondIndex == other.secondIndex && thirdIndex == other.thirdIndex ||
		firstIndex == other.secondIndex && secondIndex == other.thirdIndex && thirdIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.firstIndex && thirdIndex == other.secondIndex;
}
#pragma endregion

#pragma region MeshPrototype
MeshPrototype::MeshPrototype(const MeshPrototype& mesh) :
	vertices(copy(mesh.vertices, mesh.vertexCount)),
	triangles(copy(mesh.triangles, mesh.triangleCount)),
	vertexCount(mesh.vertexCount),
	triangleCount(mesh.triangleCount) {

}

MeshPrototype& MeshPrototype::operator=(const MeshPrototype& mesh) {
	this->vertices = copy(mesh.vertices, mesh.vertexCount);
	this->triangles = copy(mesh.triangles, mesh.triangleCount);
	this->vertexCount = mesh.vertexCount;
	this->triangleCount = mesh.triangleCount;

	return *this;
}

MeshPrototype::MeshPrototype() :
	vertices(),
	triangles(),
	vertexCount(0),
	triangleCount(0) {}

MeshPrototype::MeshPrototype(int vertexCount, int triangleCount) :
        vertices(getOffset(vertexCount) * 3, 32),
	triangles(getOffset(triangleCount) * 3, 32),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {}

MeshPrototype::MeshPrototype(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles) :
	vertices(getOffset(vertexCount) * 3, 32),
	triangles(std::move(triangles)),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {}

MeshPrototype::MeshPrototype(int vertexCount, int triangleCount, UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles) :
	vertices(std::move(vertices)),
	triangles(std::move(triangles)),
	vertexCount(vertexCount),
	triangleCount(triangleCount) {}

Vec3f MeshPrototype::getVertex(int index) const {
	assert(index >= 0 && index < vertexCount);
	size_t currect_index = (index/8)*8*2+index;//getOffset(vertexCount);
	return Vec3f(this->vertices[currect_index], this->vertices[currect_index+8], this->vertices[currect_index+8*2]);
}

Triangle MeshPrototype::getTriangle(int index) const {
	assert(index >= 0 && index < triangleCount);
	size_t currect_index = (index/8)*8*2+index;//getOffset(triangleCount);
	return Triangle{triangles[currect_index], triangles[currect_index+8], triangles[currect_index+8*2]};
}
#pragma endregion

#pragma region EditableMesh
EditableMesh::EditableMesh(int vertexCount, int triangleCount) :
	MeshPrototype(vertexCount, triangleCount) {}
EditableMesh::EditableMesh(int vertexCount, int triangleCount, const UniqueAlignedPointer<int>& triangles) :
	MeshPrototype(vertexCount, triangleCount, copy(triangles, triangleCount)) {}
EditableMesh::EditableMesh(int vertexCount, int triangleCount, UniqueAlignedPointer<int>&& triangles) :
	MeshPrototype(vertexCount, triangleCount, std::move(triangles)) {}

EditableMesh::EditableMesh(const MeshPrototype& mesh) : MeshPrototype(mesh) {}
EditableMesh::EditableMesh(MeshPrototype&& mesh) noexcept : MeshPrototype(std::move(mesh)) {}

void EditableMesh::setVertex(int index, Vec3f newVertex) {
	assert(index >= 0 && index < vertexCount);
	size_t correct_index = (index/8)*8*2+index;//getOffset(vertexCount);
	this->vertices[correct_index] = newVertex.x;
	this->vertices[correct_index + 8] = newVertex.y;
	this->vertices[correct_index + 2 * 8] = newVertex.z;
}
void EditableMesh::setVertex(int index, float x, float y, float z) {
	assert(index >= 0 && index < vertexCount);
	size_t correct_index = (index/8)*8*2+index; //getOffset(vertexCount);
	this->vertices[correct_index] = x;
	this->vertices[correct_index + 8] = y;
	this->vertices[correct_index + 2 * 8] = z;
}
void EditableMesh::setTriangle(int index, Triangle newTriangle) {
	assert(index >= 0 && index < triangleCount);
	assert(isValidTriangle(newTriangle, vertexCount));

	size_t correct_index = (index/8)*8*2+index;//getOffset(triangleCount);
	this->triangles[correct_index] = newTriangle.firstIndex;
	this->triangles[correct_index + 8] = newTriangle.secondIndex;
	this->triangles[correct_index + 2 * 8] = newTriangle.thirdIndex;
}
void EditableMesh::setTriangle(int index, int a, int b, int c) {
	assert(index >= 0 && index < triangleCount);
	assert(isValidTriangle(Triangle{a,b,c}, vertexCount));

	size_t correct_index = (index/8)*8*2+index;//getOffset(triangleCount);
	this->triangles[correct_index] = a;
	this->triangles[correct_index + 8] = b;
	this->triangles[correct_index + 2 * 8] = c;
}
#pragma endregion

#pragma region TriangleMesh
TriangleMesh::TriangleMesh(UniqueAlignedPointer<float>&& vertices, UniqueAlignedPointer<int>&& triangles, int vertexCount, int triangleCount) :
	MeshPrototype(vertexCount, triangleCount, std::move(vertices), std::move(triangles)) {
	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(int vertexCount, int triangleCount, const Vec3f* vertices, const Triangle* triangles) :
	MeshPrototype(vertexCount, triangleCount) {

        size_t vertexOffset = 8;//getOffset(vertexCount);

	
	//printf("vertex_count = ori:%d, now: %lu\ntriangle_count = ori:%d, now: %lu\n", vertexCount, getOffset(vertexCount) * 3, triangleCount, getOffset(triangleCount) * 3),
	
	//printf("TriangleMesh::TriangleMesh->vertex_offset = %lu\n", vertexOffset);
	
	//printf("TriangleMesh::TriangleMesh->vertexCount = %d\n", vertexCount);
	
	float* xValues = this->vertices.get();
	float* yValues = xValues + vertexOffset;
	float* zValues = yValues + vertexOffset;

	unsigned int a = 0;
	for(size_t i = 0; i < vertexCount; i++) {
	    xValues[a] = vertices[i].x;
	    yValues[a] = vertices[i].y;
	    zValues[a] = vertices[i].z;
	    /*  printf("x = %f\n", xValues[a]);
	    printf("y = %f\n", yValues[a]);
	    printf("z = %f\n", zValues[a]);
	    */a++;
	    a+=vertexOffset*((a%vertexOffset)==0)*2;
	    //printf("a = %u\n", a);
	    
	}

	unsigned int last_index = a-1;
	unsigned int size_left =  (((vertexCount + 7) & 0xFFFFFFFFFFFFFFF8)*3-vertexCount*3)/3;
	//	printf("a*3 = %d", a*3);
	//	printf("size_left = %u\n", size_left);
	for(unsigned int i=a; i<a+size_left; i++){
	  xValues[i] = xValues[last_index];
	  yValues[i] = yValues[last_index];
	  zValues[i] = zValues[last_index];
	  //  printf("xValues[%d] = %f\n", i, xValues[i]);
	}
	
	//fixFinalBlock(this->vertices.get(), vertexCount);

	size_t triangleOffset = 8;//getOffset(triangleCount);

	
	
	int* aValues = this->triangles.get();
	int* bValues = aValues + triangleOffset;
	int* cValues = bValues + triangleOffset;

	a = 0;
	for(size_t i = 0; i < triangleCount; i++) {
        	aValues[a] = triangles[i].firstIndex;
		bValues[a] = triangles[i].secondIndex;
		cValues[a] = triangles[i].thirdIndex;
		/*	printf("a = %d\n", aValues[a]);
		printf("b = %d\n", bValues[a]);
		printf("c = %d\n", cValues[a]);
		*/a++;
		a+=triangleOffset*((a%triangleOffset)==0)*2;
	        //printf("a_index = %u\n", a);
	}

	
	last_index = a-1;
	size_left =  ((((triangleCount + 7) & 0xFFFFFFFFFFFFFFF8)*3-triangleCount*3)/3);
	//printf("a*3 = %d", a*3);
	//printf("size_left = %u\n", size_left);
	for(unsigned int i=a; i<a+size_left; i++){
	  aValues[i] = aValues[last_index];
	  bValues[i] = bValues[last_index];
	  cValues[i] = cValues[last_index];
	  /*printf("aValues[%d] = %d\n", i, aValues[i]);
	  printf("bValues[%d] = %d\n", i, bValues[i]);
	  printf("cValues[%d] = %d\n", i, cValues[i]);
	  */}
		
	//fixFinalBlock(this->triangles.get(), triangleCount);

	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(const MeshPrototype& mesh) :
	MeshPrototype(mesh) {
	fixFinalBlock(this->vertices.get(), vertexCount);
	fixFinalBlock(this->triangles.get(), triangleCount);
	assert(isValid(*this));
}

TriangleMesh::TriangleMesh(MeshPrototype&& mesh) noexcept :
	MeshPrototype(std::move(mesh)) {
	fixFinalBlock(this->vertices.get(), vertexCount);
	fixFinalBlock(this->triangles.get(), triangleCount);
	assert(isValid(*this));
}

IteratorFactory<ShapeVertexIter> TriangleMesh::iterVertices() const {
  return IteratorFactory<ShapeVertexIter>(ShapeVertexIter{vertices, 8, 0},
					  ShapeVertexIter{&vertices[getOffset(vertexCount) * 3 -
								    ((getOffset(vertexCount)*3-vertexCount*3)/3) - 8*2 -
								    1 * (vertexCount%8==0)], 8, 0});
}
IteratorFactory<ShapeTriangleIter> TriangleMesh::iterTriangles() const {
  return IteratorFactory<ShapeTriangleIter>(ShapeTriangleIter{triangles, 8, 0},
					    ShapeTriangleIter{&triangles[getOffset(triangleCount) *3 -
									 ((getOffset(triangleCount)*3-triangleCount*3)/3) - 8*2 -
									 1 * (triangleCount%8==0)], 8, 0});
}

void TriangleMesh::getTriangles(Triangle* triangleBuf) const {
	size_t i = 0;
	for(Triangle triangle : iterTriangles()) {
		triangleBuf[i++] = triangle;
	}
}
void TriangleMesh::getVertices(Vec3f* vertexBuf) const {
	size_t i = 0;
	for(Vec3f vertex : iterVertices()) {
		vertexBuf[i++] = vertex;
	}
}


TriangleMesh TriangleMesh::translated(Vec3f offset) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, this->getVertex(i) + offset);
	}
	return TriangleMesh(std::move(result));
}

TriangleMesh TriangleMesh::rotated(Rotationf rotation) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, rotation * this->getVertex(i));
	}
	return TriangleMesh(std::move(result));
}

TriangleMesh TriangleMesh::localToGlobal(CFramef frame) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, frame.localToGlobal(this->getVertex(i)));
	}
	return TriangleMesh(std::move(result));
}

TriangleMesh TriangleMesh::globalToLocal(CFramef frame) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		result.setVertex(i, frame.globalToLocal(this->getVertex(i)));
	}
	return TriangleMesh(std::move(result));
}
TriangleMesh TriangleMesh::scaled(float scaleX, float scaleY, float scaleZ) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f v = this->getVertex(i);
		result.setVertex(i, Vec3f(scaleX * v.x, scaleY * v.y, scaleZ * v.z));
	}
	return TriangleMesh(std::move(result));
}
TriangleMesh TriangleMesh::scaled(DiagonalMat3f scale) const {
	return scaled(scale[0], scale[1], scale[2]);
}

TriangleMesh TriangleMesh::translatedAndScaled(Vec3f translation, DiagonalMat3f scale) const {
	EditableMesh result(this->vertexCount, this->triangleCount, this->triangles);
	for(int i = 0; i < this->vertexCount; i++) {
		Vec3f cur = this->getVertex(i);
		result.setVertex(i, scale * (cur + translation));
	}
	return TriangleMesh(std::move(result));
}


Vec3f TriangleMesh::getNormalVecOfTriangle(Triangle triangle) const {
	Vec3f v0 = this->getVertex(triangle.firstIndex);
	return (this->getVertex(triangle.secondIndex) - v0) % (this->getVertex(triangle.thirdIndex) - v0);
}

void TriangleMesh::computeNormals(Vec3f* buffer) const {
	// TODO parallelize
	for (Triangle triangle : iterTriangles()) {
		Vec3f v0 = this->getVertex(triangle.firstIndex);
		Vec3f v1 = this->getVertex(triangle.secondIndex);
		Vec3f v2 = this->getVertex(triangle.thirdIndex);

		Vec3f D10 = normalize(v1 - v0);
		Vec3f D20 = normalize(v2 - v0);
		Vec3f D21 = normalize(v2 - v1);

		buffer[triangle.firstIndex] += D10 % D20;
		buffer[triangle.secondIndex] += D10 % D21;
		buffer[triangle.thirdIndex] += D20 % D21;
	}

	for (int i = 0; i < vertexCount; i++) {
		buffer[i] = normalize(buffer[i]);
	}
}


CircumscribingSphere TriangleMesh::getCircumscribingSphere() const {
	BoundingBox bounds = getBounds();
	Vec3 center = bounds.getCenter();
	double radius = getMaxRadius(center);
	return CircumscribingSphere{center, radius};
}

double TriangleMesh::getMaxRadiusSq() const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex);
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double TriangleMesh::getMaxRadiusSq(Vec3f reference) const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(vertex - reference);
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}

double TriangleMesh::getMaxRadius() const {
	return sqrt(getMaxRadiusSq());
}

double TriangleMesh::getMaxRadius(Vec3f reference) const {
	return sqrt(getMaxRadiusSq(reference));
}


double TriangleMesh::getScaledMaxRadiusSq(DiagonalMat3 scale) const {
	double bestDistSq = 0;
	for(Vec3f vertex : iterVertices()) {
		double distSq = lengthSquared(scale * Vec3(vertex));
		if(distSq > bestDistSq) {
			bestDistSq = distSq;
		}
	}
	return bestDistSq;
}
double TriangleMesh::getScaledMaxRadius(DiagonalMat3 scale) const {
	return sqrt(getScaledMaxRadiusSq(scale));
}

double TriangleMesh::getIntersectionDistance(const Vec3& origin, const Vec3& direction) const {
	const double EPSILON = 0.0000001;
	double t = std::numeric_limits<double>::max();
	for(Triangle triangle : iterTriangles()) {
		Vec3 v0 = this->getVertex(triangle.firstIndex);
		Vec3 v1 = this->getVertex(triangle.secondIndex);
		Vec3 v2 = this->getVertex(triangle.thirdIndex);

		Vec3 edge1 = v1 - v0;
		Vec3 edge2 = v2 - v0;
		Vec3 h = direction % edge2;

		double a = edge1 * h;
		if(a > -EPSILON && a < EPSILON)
			continue;

		Vec3 s = origin - v0;
		double f = 1.0 / a;
		double u = f * (s * h);

		if(u < 0.0 || u > 1.0)
			continue;

		Vec3 q = s % edge1;
		double v = direction * f * q;

		if(v < 0.0 || u + v > 1.0)
			continue;

		double r = edge2 * f * q;
		if(r > EPSILON) {
			if(r < t)
				t = r;
		} else {
			//Log::debug("Line intersection but not a ray intersection");
			continue;
		}
	}

	return t;
}


TriangleMesh stripUnusedVertices(const Vec3f* vertices, const Triangle* triangles, int vertexCount, int triangleCount) {
	bool* vertexIsReferenced = new bool[vertexCount];
	for(int i = 0; i < vertexCount; i++) { vertexIsReferenced[i] = false; }
	for(int i = 0; i < triangleCount; i++) {
		Triangle t = triangles[i];
		vertexIsReferenced[t.firstIndex] = true;
		vertexIsReferenced[t.secondIndex] = true;
		vertexIsReferenced[t.thirdIndex] = true;
	}
	int totalUnReferencedCount = 0;
	for(int i = 0; i < vertexCount; i++) {
		if(vertexIsReferenced[i] == false) {
			totalUnReferencedCount++;
		}
	}
	int lastValidVertex = vertexCount - 1;
	while(vertexIsReferenced[lastValidVertex] == false) {
		lastValidVertex--;
	}
	std::vector<std::pair<int, int>> substitutions(totalUnReferencedCount); // substitute first with second in triangles
	EditableMesh result(vertexCount - totalUnReferencedCount, triangleCount);
	// fix vertices
	for(int i = 0; i <= lastValidVertex; i++) {
		if(vertexIsReferenced[i] == false) {
			result.setVertex(i, vertices[lastValidVertex]);
			substitutions.push_back(std::make_pair(lastValidVertex, i));
			do {
				lastValidVertex--;
			} while(vertexIsReferenced[lastValidVertex] == false && lastValidVertex > i); // the second condition is for the case that the first n vertices are all invalid, which would make lastValidVertex run off 0
		} else {
			result.setVertex(i, vertices[i]);
		}
	}
	delete[] vertexIsReferenced;
	for(int i = 0; i < triangleCount; i++) {
		Triangle t = triangles[i];
		for(std::pair<int, int>& sub : substitutions) {
			if(t.firstIndex == sub.first) t.firstIndex = sub.second;
			if(t.secondIndex == sub.first) t.secondIndex = sub.second;
			if(t.thirdIndex == sub.first) t.thirdIndex = sub.second;
		}
		result.setTriangle(i, t);
	}
	return TriangleMesh(std::move(result));
}

int TriangleMesh::furthestIndexInDirectionFallback(const Vec3f& direction) const {
	float bestDot = this->getVertex(0) * direction;
	int bestVertexIndex = 0;
	for(int i = 1; i < vertexCount; i++) {
		float newD = this->getVertex(i) * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertexIndex = i;
		}
	}

	return bestVertexIndex;
}

Vec3f TriangleMesh::furthestInDirectionFallback(const Vec3f& direction) const {
	float bestDot = this->getVertex(0) * direction;
	Vec3f bestVertex = this->getVertex(0);
	for(int i = 1; i < vertexCount; i++) {
		float newD = this->getVertex(i) * direction;
		if(newD > bestDot) {
			bestDot = newD;
			bestVertex = this->getVertex(i);
		}
	}

	return bestVertex;
}


BoundingBox TriangleMesh::getBoundsFallback() const {
	double xmin = this->getVertex(0).x, xmax = this->getVertex(0).x;
	double ymin = this->getVertex(0).y, ymax = this->getVertex(0).y;
	double zmin = this->getVertex(0).z, zmax = this->getVertex(0).z;

	for(int i = 1; i < vertexCount; i++) {
		const Vec3f current = this->getVertex(i);

		if(current.x < xmin) xmin = current.x;
		if(current.x > xmax) xmax = current.x;
		if(current.y < ymin) ymin = current.y;
		if(current.y > ymax) ymax = current.y;
		if(current.z < zmin) zmin = current.z;
		if(current.z > zmax) zmax = current.z;
	}

	return BoundingBox{xmin, ymin, zmin, xmax, ymax, zmax};
}

BoundingBox TriangleMesh::getBoundsFallback(const Mat3f& referenceFrame) const {
	Mat3f transp = referenceFrame.transpose();
	double xmax = (referenceFrame * this->furthestInDirection(transp * Vec3f(1, 0, 0))).x;
	double xmin = (referenceFrame * this->furthestInDirection(transp * Vec3f(-1, 0, 0))).x;
	double ymax = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 1, 0))).y;
	double ymin = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, -1, 0))).y;
	double zmax = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 0, 1))).z;
	double zmin = (referenceFrame * this->furthestInDirection(transp * Vec3f(0, 0, -1))).z;

	return BoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);
}

int TriangleMesh::furthestIndexInDirection(const Vec3f& direction) const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return furthestIndexInDirectionAVX(direction);
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE4_1)) {
			return furthestIndexInDirectionSSE4(direction);
		} else {
			return furthestIndexInDirectionSSE(direction);
		}
	} else {
		return furthestIndexInDirectionFallback(direction);
	}
}

Vec3f TriangleMesh::furthestInDirection(const Vec3f& direction) const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return furthestInDirectionAVX(direction);
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE4_1)) {
			return furthestInDirectionSSE4(direction);
		} else {
			return furthestInDirectionSSE(direction);
		}
	} else {
		return furthestInDirectionFallback(direction);
	}
}

BoundingBox TriangleMesh::getBounds() const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return getBoundsAVX();
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		return getBoundsSSE();
	} else {
		return getBoundsFallback();
	}
}

BoundingBox TriangleMesh::getBounds(const Mat3f& referenceFrame) const {
	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		return getBoundsAVX(referenceFrame);
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		return getBoundsSSE(referenceFrame);
	} else {
		return getBoundsFallback(referenceFrame);
	}
}

#pragma endregion
};
