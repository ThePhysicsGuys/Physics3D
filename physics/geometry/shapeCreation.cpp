#include "shapeCreation.h"

#include "shapeClass.h"
#include "polyhedron.h"
#include "../misc/shapeLibrary.h"
#include "../math/linalg/trigonometry.h"
#include "builtinShapeClasses.h"

Shape sphereShape(double radius) {
	return Shape(&SphereClass::instance, radius * 2, radius * 2, radius * 2);
}

Shape cylinderShape(double radius, double height) {
	return Shape(&CylinderClass::instance, radius * 2, radius * 2, height);
}

Shape boxShape(double width, double height, double depth) {
	return Shape(&CubeClass::instance, width, height, depth);
}

Shape polyhedronShape(const Polyhedron& poly) {
	BoundingBox bounds = poly.getBounds();
	Vec3 center = bounds.getCenter();
	DiagonalMat3 scale{2 / bounds.getWidth(), 2 / bounds.getHeight(), 2 / bounds.getDepth()};

	PolyhedronShapeClass* shapeClass = new PolyhedronShapeClass(poly.translatedAndScaled(-center, scale));

	return Shape(shapeClass, bounds.getWidth(), bounds.getHeight(), bounds.getDepth());
}
