#include "shapeCreation.h"

#include "shapeClass.h"
#include "polyhedron.h"
#include "builtinShapeClasses.h"

#include "../misc/cpuid.h"

#include "../datastructures/smartPointers.h"

namespace P3D {
Shape boxShape(double width, double height, double depth) {
	return Shape(intrusive_ptr<const ShapeClass>(&CubeClass::instance), width, height, depth);
}

Shape wedgeShape(double width, double height, double depth) {
	return Shape(intrusive_ptr<const ShapeClass>(&WedgeClass::instance), width, height, depth);
}

Shape cornerShape(double width, double height, double depth) {
	return Shape(intrusive_ptr<const ShapeClass>(&CornerClass::instance), width, height, depth);
}

Shape sphereShape(double radius) {
	return Shape(intrusive_ptr<const ShapeClass>(&SphereClass::instance), radius * 2, radius * 2, radius * 2);
}

Shape cylinderShape(double radius, double height) {
	return Shape(intrusive_ptr<const ShapeClass>(&CylinderClass::instance), radius * 2, radius * 2, height);
}


Shape polyhedronShape(const Polyhedron& poly) {
	BoundingBox bounds = poly.getBounds();
	Vec3 center = bounds.getCenter();
	DiagonalMat3 scale{2 / bounds.getWidth(), 2 / bounds.getHeight(), 2 / bounds.getDepth()};

	PolyhedronShapeClass* shapeClass;

	if(CPUIDCheck::hasTechnology(CPUIDCheck::AVX | CPUIDCheck::AVX2 | CPUIDCheck::FMA)) {
		shapeClass = new PolyhedronShapeClassAVX(poly.translatedAndScaled(-center, scale));
	} else if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE | CPUIDCheck::SSE2)) {
		if(CPUIDCheck::hasTechnology(CPUIDCheck::SSE4_1)) {
			shapeClass = new PolyhedronShapeClassSSE4(poly.translatedAndScaled(-center, scale));
		} else {
			shapeClass = new PolyhedronShapeClassSSE(poly.translatedAndScaled(-center, scale));
		}
	} else {
		shapeClass = new PolyhedronShapeClassFallback(poly.translatedAndScaled(-center, scale));
	}

	return Shape(intrusive_ptr<const ShapeClass>(shapeClass), bounds.getWidth(), bounds.getHeight(), bounds.getDepth());
}
};