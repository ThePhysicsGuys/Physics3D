#include "shapeClass.h"

#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

#include "basicShapes.h"
#include "normalizedPolyhedron.h"
#include "../misc/shapeLibrary.h"
#include "../math/linalg/trigonometry.h"




struct CubeClass : public ShapeClass {
	CubeClass() : ShapeClass(8, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(8.0 / 3.0, 8.0 / 3.0, 8.0 / 3.0), Vec3(0, 0, 0)), CUBE_CLASS_ID) {};

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
		Vec3f result(direction.x < 0 ? -1.0f : 1.0f, direction.y < 0 ? -1.0f : 1.0f, direction.z < 0 ? -1.0f : 1.0f);
		return polyresult;
	}

	virtual Polyhedron asPolyhedron() const {
		return Library::createCube(2.0);
	}
};

struct SphereClass : public ShapeClass {
	SphereClass() : ShapeClass(4.0 / 3.0 * M_PI, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(4.0 / 15.0 * M_PI, 4.0 / 15.0 * M_PI, 4.0 / 15.0 * M_PI), Vec3(0, 0, 0)), SPHERE_CLASS_ID) {};

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

	virtual double getScaledMaxRadius(DiagonalMat3 scale) const {
		return scale[0];
	}

	virtual Vec3f furthestInDirection(const Vec3f& direction) const {
		return normalize(direction);
	}

	virtual Polyhedron asPolyhedron() const {
		return Library::createSphere(1.0, 3);
	}

	void setScaleX(double newX, DiagonalMat3& scale) const override {
		scale[0] = newX;
		scale[1] = newX;
		scale[2] = newX;
	}
	void setScaleY(double newY, DiagonalMat3& scale) const override {
		scale[0] = newY;
		scale[1] = newY;
		scale[2] = newY;
	}
	void setScaleZ(double newZ, DiagonalMat3& scale) const override {
		scale[0] = newZ;
		scale[1] = newZ;
		scale[2] = newZ;
	}
};

struct CylinderClass : public ShapeClass {
	/*
	Inertia of cyllinder: 
		z = V * 1/2
		x, y = V * 7/12

		X = y + z = 7/12
		Y = x + z = 7/12
		Z = x + y = 1/2

		x = 1/4 * M_PI * 2
		y = 1/4 * M_PI * 2
		z = 1/3 * M_PI * 2
	*/

	CylinderClass() : ShapeClass(M_PI * 2.0, Vec3(0, 0, 0), ScalableInertialMatrix(Vec3(M_PI / 2.0, M_PI / 2.0, M_PI * 2.0/3.0), Vec3(0, 0, 0)), CYLINDER_CLASS_ID) {};

	virtual bool containsPoint(Vec3 point) const {
		return abs(point.z) <= 1.0 && point.x * point.x + point.y + point.y <= 1.0;
	}

	virtual double getIntersectionDistance(Vec3 origin, Vec3 direction) const {
		Vec2 xyOrigin(origin.x, origin.y);
		Vec2 xyDirection(direction.x, direction.y);

		//o*o + o*d*t + o*d*t + t*t*d*d
		double c = xyOrigin * xyOrigin - 1;
		double b = xyOrigin * xyDirection;
		double a = xyDirection * xyDirection;

		// solve a*t^2 + 2*b*t + c
		double D = b * b - a * c;
		if(D >= 0) {
			double t = (-b + -sqrt(D)) / a;
			double z = origin.z + t * direction.z;
			if(abs(z) <= 1.0) {
				return t;
			} else {
				// origin + t * direction = 1 => t = (1-origin)/direction
				
				double t2 = (((origin.z >= 0)?1:-1) - origin.z) / direction.z;

				double x = origin.x + t2 * direction.x;
				double y = origin.y + t2 * direction.y;

				if(x * x + y * y <= 1.0) {
					return t2;
				} else {
					return INFINITY;
				}
			}
		} else {
			return INFINITY;
		}
	}

	virtual BoundingBox getBounds(const RotMat3& rotation, const DiagonalMat3& scale) const {
		double height = scale[2];
		double radius = scale[0];

		Vec3 normalizedZVector = abs(rotation.getCol(2));
		Vec3 zVector = normalizedZVector * height;

		double extraX = sqrt(1 - normalizedZVector.x * normalizedZVector.x);
		double extraY = sqrt(1 - normalizedZVector.y * normalizedZVector.y);
		double extraZ = sqrt(1 - normalizedZVector.z * normalizedZVector.z);

		double x = zVector.x + extraX * radius;
		double y = zVector.y + extraY * radius;
		double z = zVector.z + extraZ * radius;

		return BoundingBox{-x, -y, -z, x, y, z};
	}

	virtual double getScaledMaxRadiusSq(DiagonalMat3 scale) const {
		return scale[0] * scale[0] + scale[2] * scale[2];
	}

	virtual Vec3f furthestInDirection(const Vec3f& direction) const {
		float normalizer = sqrt(direction.x * direction.x + direction.y * direction.y);
		return Vec3f(direction.x / normalizer, direction.y / normalizer, (direction.z >= 0.0f) ? 1.0f : -1.0f);
	}

	virtual Polyhedron asPolyhedron() const {
		return Library::createZPrism(64, 1.0, 2.0);
	}

	void setScaleX(double newX, DiagonalMat3& scale) const override {
		scale[0] = newX;
		scale[1] = newX;
	}
	void setScaleY(double newY, DiagonalMat3& scale) const override {
		scale[0] = newY;
		scale[1] = newY;
	}
};

static const CubeClass box;
static const SphereClass sphere;
static const CylinderClass cylinder;


const ShapeClass* const sphereClass = &sphere;
const ShapeClass* const boxClass = &box;
const ShapeClass* const cylinderClass = &cylinder;


ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, int intersectionClassID) : 
	volume(volume), 
	centerOfMass(centerOfMass), 
	inertia(inertia), 
	intersectionClassID(intersectionClassID) {}


double ShapeClass::getScaledMaxRadius(DiagonalMat3 scale) const {
	return sqrt(this->getScaledMaxRadiusSq(scale));
}


void ShapeClass::setScaleX(double newX, DiagonalMat3& scale) const {
	scale[0] = newX;
}
void ShapeClass::setScaleY(double newY, DiagonalMat3& scale) const {
	scale[1] = newY;
}
void ShapeClass::setScaleZ(double newZ, DiagonalMat3& scale) const {
	scale[2] = newZ;
}

Shape Sphere(double radius) {
	return Shape(&sphere, radius * 2, radius * 2, radius * 2);
}

Shape Cylinder(double radius, double height) {
	return Shape(&cylinder, radius * 2, radius * 2, height);
}

Shape Box(double width, double height, double depth) {
	return Shape(&box, width, height, depth);
}

/*Shape Wedge(double width, double height, double depth) {

}*/

