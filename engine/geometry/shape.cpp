#include "shape.h"

#include <stdlib.h>
#include <cstring>

#include "../../util/Log.h"

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
	return Triangle{firstIndex, thirdIndex, secondIndex};
}

bool Triangle::operator==(const Triangle & other) const {
	return firstIndex == other.firstIndex && secondIndex == other.secondIndex && thirdIndex == other.thirdIndex || 
		firstIndex == other.secondIndex && secondIndex == other.thirdIndex && thirdIndex == other.firstIndex ||
		firstIndex == other.thirdIndex && secondIndex == other.firstIndex && thirdIndex == other.secondIndex;
}

Triangle Triangle::rightShift() const {return Triangle{thirdIndex, firstIndex, secondIndex};}
Triangle Triangle::leftShift() const { return Triangle{secondIndex, thirdIndex, firstIndex};}

Shape::Shape() : vertices(nullptr), triangles(nullptr), vCount(0), tCount(0) {}

Shape::Shape(const Vec3 * vertices, const Triangle * triangles, int vCount, int tCount) : vertices(vertices), triangles(triangles), vCount(vCount), tCount(tCount) {}

Shape Shape::translated(Vec3 offset, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = offset + vertices[i];
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::rotated(RotMat3 rotation, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = rotation * vertices[i];
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::localToGlobal(CFrame frame, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = frame.localToGlobal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

Shape Shape::globalToLocal(CFrame frame, Vec3 * newVecBuf) const {
	for (int i = 0; i < this->vCount; i++) {
		newVecBuf[i] = frame.globalToLocal(vertices[i]);
	}

	return Shape(newVecBuf, triangles, vCount, tCount);
}

BoundingBox Shape::getBounds() const {
	double xmin = vertices[0].x, xmax = vertices[0].x;
	double ymin = vertices[0].y, ymax = vertices[0].y;
	double zmin = vertices[0].z, zmax = vertices[0].z;

	for (int i = 1; i < vCount; i++) {
		const Vec3 cur = vertices[i];

		if (cur.x < xmin) xmin = cur.x;
		if (cur.x > xmax) xmax = cur.x;
		if (cur.y < ymin) ymin = cur.y;
		if (cur.y > ymax) ymax = cur.y;
		if (cur.z < zmin) zmin = cur.z;
		if (cur.z > zmax) zmax = cur.z;
	}

	return BoundingBox{xmin, ymin, zmin, xmax, ymax, zmax};
}

bool isComplete(const Triangle* triangles, int tCount);
bool Shape::isValid() const {
	return isComplete(triangles, tCount);
}

// for every edge, of every triangle, check that it coincides with exactly one other triangle, in reverse order
bool isComplete(const Triangle* triangles, int tCount) {
	for (int i = 0; i < tCount; i++) {
		Triangle a = triangles[i];
		
		for (int j = 0; j < tCount; j++) {
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

double Shape::getVolume() const {
	return 0.0;
}
