#include "shapeClass.h"

#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

#include "basicShapes.h"
#include "normalizedPolyhedron.h"
#include "../misc/shapeLibrary.h"
#include "../math/linalg/trigonometry.h"

static struct CubeClass : public ShapeClass {
	CubeClass() : ShapeClass(8, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(8.0 / 3.0, 8.0 / 3.0, 8.0 / 3.0), Vec3(0, 0, 0)), 0) {};

	virtual bool containsPoint(Vec3 point) const {
		return abs(point.x) <= 1.0 && abs(point.y) <= 1.0 && abs(point.z) <= 1.0;
	}
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const {
		if(origin.x < 0) {
			origin.x = -origin.x;
			direction.x = -direction.x;
		}
		if(origin.y < 0) {
			origin.y = -origin.y;
			direction.y = -direction.y;
		}
		if(origin.z < 0) {
			origin.z = -origin.z;
			direction.z = -direction.z;
		}

		//origin + t * direction = x1z
		double tx = (1-origin.x) / direction.x;

		Vec3 intersX = origin + tx * direction;
		if(abs(intersX.y) <= 1.0 && abs(intersX.z) <= 1.0) return tx;

		double ty = (1-origin.y) / direction.y;

		Vec3 intersY = origin + ty * direction;
		if(abs(intersY.x) <= 1.0 && abs(intersY.z) <= 1.0) return ty;

		double tz = (1-origin.z) / direction.z;

		Vec3 intersZ = origin + tz * direction;
		if(abs(intersZ.x) <= 1.0 && abs(intersZ.y) <= 1.0) return tz;

		return INFINITY;
	}

	virtual BoundingBox getBounds(const RotMat3& rotation, const DiagonalMat3& scale) const {
		Mat3 referenceFrame = rotation * scale;
		double x = abs(referenceFrame[0][0]) + abs(referenceFrame[0][1]) + abs(referenceFrame[0][2]);
		double y = abs(referenceFrame[1][0]) + abs(referenceFrame[1][1]) + abs(referenceFrame[1][2]);
		double z = abs(referenceFrame[2][0]) + abs(referenceFrame[2][1]) + abs(referenceFrame[2][2]);
		BoundingBox result{-x,-y,-z,x,y,z};
		return result;
	}
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const {
		return scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2];
	}

	virtual Vec3f furthestInDirection(const Vec3f& direction) const {
		Vec3f polyresult = asPolyhedron().furthestInDirection(direction);
		Vec3f result(direction.x < 0 ? -1 : 1, direction.y < 0 ? -1 : 1, direction.z < 0 ? -1 : 1);
		return polyresult;
	}

	virtual Polyhedron asPolyhedron() const {
		return Library::createCube(2.0);
	}
};

static struct SphereClass : public ShapeClass {
	SphereClass() : ShapeClass(4.0 / 3.0 * M_PI, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(4.0 / 15.0 * M_PI, 4.0 / 15.0 * M_PI, 4.0 / 15.0 * M_PI), Vec3(0, 0, 0)), 1) {};

	virtual bool containsPoint(Vec3 point) const {
		return lengthSquared(point) <= 1.0;
	}
	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const {
		//o*o + o*d*t + o*d*t + t*t*d*d
		double c = origin * origin - 1;
		double b = origin * direction;
		double a = direction * direction;

		// solve a*t^2 + 2*b*t + c
		double D = b * b - a * c;
		if(D >= 0) {
			return (-b + -sqrt(D)) / a;
		} else {
			return INFINITY;
		}
	}

	virtual BoundingBox getBounds(const RotMat3& rotation, const DiagonalMat3& scale) const {
		double s = scale[0];
		return BoundingBox{-s, -s, -s, s, s, s};
	}
	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const {
		return scale[0] * scale[0];
	}

	virtual Vec3f furthestInDirection(const Vec3f& direction) const {
		return normalize(direction);
	}

	virtual Polyhedron asPolyhedron() const {
		return Library::createSphere(2.0, 3);
	}
};

static const CubeClass box;
static const SphereClass sphere;

static std::map<int, const ShapeClass*> knownShapeClasses{{0, &box}, {1, &sphere}};

int newClassID() {
	if(knownShapeClasses.empty()) return 0;
	for(int i = 0; ; i++) {
		if(knownShapeClasses.find(i) == knownShapeClasses.end()) {
			return i;
		}
	}
}

ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, int classID) : volume(volume), centerOfMass(centerOfMass), inertia(inertia), classID(classID) {}
ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia) : ShapeClass(volume, centerOfMass, inertia, newClassID()) {
	knownShapeClasses.insert(std::pair<int, const ShapeClass*>(this->classID, this));
}


const ShapeClass* ShapeClass::getShapeClassForId(int classID) {
	return knownShapeClasses[classID];
}

double ShapeClass::getScaledMaxRadius(DiagonalMat3 scale) const {
	return sqrt(this->getScaledMaxRadiusSq(scale));
}

// radius 1 sphere
//static ShapeClass sphere()

//static const NormalizedPolyhedron sphere(Library::createSphere(1.0, 2).normalized());

Shape Sphere(double radius) {
	return Shape(&sphere, radius * 2, radius * 2, radius * 2);
}

/*Shape Cyllinder(double radius, double height) {

}*/

Shape Box(double width, double height, double depth) {
	return Shape(&box, width, height, depth);
}

/*Shape Wedge(double width, double height, double depth) {

}*/

